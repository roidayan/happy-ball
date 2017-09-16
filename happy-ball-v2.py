#!/usr/bin/python


import time
import os
import asyncore
import socket
from glob import glob
import random
import time
from threading import Thread


random.seed()

audio_move = []
audio_idle = glob('audio/idle/*.wav')
audio_start = glob('audio/start/*.wav')

idle_time = 1
idle_talk = 4


def play(a):
    os.system('aplay ' + a)

TALK = 1
IDLE = 2
state = IDLE
last_move = time.time()
idle_count = 0


def talk1():
	a = random.choice(audio_start)
	play(a)

def talk_move():
	global audio_move
	if not audio_move:
		audio_move = glob('audio/move/*.wav')
	a = random.choice(audio_move)
	play(a)
	audio_move.remove(a)

def talk_idle():
	a = random.choice(audio_idle)
	play(a)


def stateThread():
	global state
	global last_move
	global idle_count
	while True:
		if time.time() - last_move > idle_time:
			print 'back to idle'
			state = IDLE
		if state == TALK:
			print 'talk'
			talk_move()
			time.sleep(0.6)
			idle_count = 0
		elif idle_count < 3:
			if ((time.time() - last_move) >= idle_talk):
				talk_idle()
				last_move = time.time()
				idle_count+=1
		time.sleep(0.1)


class EchoServer(asyncore.dispatcher):

    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.set_reuse_addr()
        self.bind((host, port))

    def handle_read(self):
	global state
	global last_move
        data, addr = self.recvfrom(2048)
        if data:
		print data
		if data == 'move':
			state = TALK
			last_move = time.time()
		else:
			state = IDLE


t1 = Thread(target=stateThread)
t1.start()
server = EchoServer('', 2020)
asyncore.loop()
t1.join()
