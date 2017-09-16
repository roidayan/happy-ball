#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <stdlib.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif
 
#define LED_PIN LED_BUILTIN
 
const char* ssid = "AndroidAP";
const char* password = "klein1234";

bool blinkState = false;
bool motion;

WiFiUDP Udp;
const IPAddress outIp(192, 168, 43, 56);
const unsigned int outPort = 2020;

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high


void sendUdpPacket(const char* msg)
{
    Udp.beginPacket(outIp, outPort);
    Udp.write(msg);
    Udp.endPacket();
}

void program()
{
  motion = accelgyro.getIntMotionStatus();

  if (motion) {
    sendUdpPacket("move");
  }

  delay(50);

  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);
}
 
 
void setup() {
  pinMode(LED_PIN, OUTPUT);
 
  Serial.begin(9600);             
  //Serial.setDebugOutput(true);

  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf("WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Set the hostname
  WiFi.hostname("web-blink");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  //Begin I2C data transfer
  //Wire.begin(D1, D2);                 // SCL <==> D1 , SDA <==> D2
  Wire.begin(4, 5);                 // SCL <==> D1 , SDA <==> D2
  
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  accelgyro.setAccelerometerPowerOnDelay(3);
  accelgyro.setIntZeroMotionEnabled(false);
  accelgyro.setDHPFMode(1);

  accelgyro.setMotionDetectionThreshold(5);
  accelgyro.setMotionDetectionDuration(25);

  accelgyro.setZeroMotionDetectionThreshold(10);
  accelgyro.setZeroMotionDetectionDuration(5);
  
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  pinMode(LED_PIN, OUTPUT);
 }
 
 void loop()
 {
   if (WiFi.status() == WL_CONNECTED) {
     program();
   } else {
     Serial.println("Unable to connect wifi");
   }
 }