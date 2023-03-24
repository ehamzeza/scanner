#include <Wire.h>
#include "SparkFun_VL53L1X.h"

SFEVL53L1X distanceSensor;

void scanLine(uint32_t num_points, float spacing)
{
//  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
//  while (!distanceSensor.checkForDataReady())
//  {
//    delay(1);
//  }
//  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
//  distanceSensor.clearInterrupt();
//  distanceSensor.stopRanging();
//
//  Serial.print("Distance(mm): ");
//  Serial.print(distance);
//
//  float distanceInches = distance * 0.0393701;
//  float distanceFeet = distanceInches / 12.0;
//
//  Serial.print("\tDistance(ft): ");
//  Serial.print(distanceFeet, 2);
//
//  Serial.println();
}

void debugLog(String message)
{
  uint32_t message_length = (uint32_t) message.length();
  Serial.print('$');
  char s[4];
  memcpy(s, &message_length, 4);
  Serial.write(s, 4);
  Serial.print(message);
  Serial.flush();
}

void setup(void)
{
  Wire.begin();
  Serial.begin(256000);

  if (distanceSensor.begin()) {
    debugLog("TOF Sensor Failed!");
  }

  delay(1000);
  debugLog("Scanner online!");
}

void loop(void)
{
  if (Serial.available() <= 0)
    return;
  
  switch (Serial.peek()) {
    case 'r':
      Serial.read();
      debugLog("Ready!");
      break;

    case 'z':
      if (Serial.available() >= 2) {
        Serial.read();
        char axis = Serial.read();
        if (axis == 'z') {
          delay(1000);
          debugLog("Zeroed Z");
        } else if (axis == 'r') {
          delay(200);
          debugLog("Zeroed R");
        } else {
          debugLog("Unknown Axis");
        }
      }
      break;

    case 'h':
      Serial.read();
      delay(1000);
      debugLog("Height 100mm");
      break;

    case 'g':
      if (Serial.available() >= 6) {
        Serial.read();
        char axis = Serial.read();
        float pos = 0.0f;
        Serial.readBytes((uint8_t*) &pos, sizeof(float));
        delay(500);
        debugLog("Moving Axis '" + String(axis) + "' to " + String(pos));
      }
      break;

    case 's':
      if (Serial.available() >= 9) {
        Serial.read();
        uint32_t num_points = 0;
        Serial.readBytes((uint8_t*) &num_points, sizeof(uint32_t));
        float spacing = 0.0f;
        Serial.readBytes((uint8_t*) &spacing, sizeof(float));
        
        if (num_points > 0) {
          debugLog("Scanning " + String(num_points) + " points, spacing = " + String(spacing));
          scanLine(num_points, spacing);
        } else {
          debugLog("Invalid Scan Request!");
        }
      }
      break;
      
    default:
      debugLog("Unknown Arduino Char '" + String((char)Serial.read()) + "'!");
      break;
  };
}
