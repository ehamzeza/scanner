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

void setup(void)
{
  Wire.begin();
  Serial.begin(256000);

  Serial.println("Initializing TOF Sensor...");
  if (distanceSensor.begin()) {
    Serial.println("TOF Sensor Failed!");
  }
  Serial.println("Sensor online!");
}

void loop(void)
{
  if (Serial.available() <= 0)
    return;
  
  switch (Serial.peek()) {
    case 'r':
      Serial.read();
      Serial.println("Ready!");
      break;

    case 'z':
      if (Serial.available() >= 2) {
        Serial.read();
        char axis = Serial.read();
        if (axis == 'z') {
          delay(1000);
          Serial.println("Zeroed Z");
        } else if (axis == 'r') {
          delay(200);
          Serial.println("Zeroed R");
        } else {
          Serial.println("Unknown Axis");
        }
      }
      break;

    case 'h':
      Serial.read();
      delay(1000);
      Serial.println("Height 100mm");
      break;

    case 'g':
      if (Serial.available() >= 6) {
        Serial.read();
        char axis = Serial.read();
        float pos = 0.0f;
        Serial.readBytes((uint8_t*) &pos, sizeof(float));
        delay(500);
        Serial.println("Moving Axis '" + String(axis) + "' to " + String(pos));
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
          Serial.println("Scanning " + String(num_points) + " points, spacing = " + String(spacing));
          scanLine(num_points, spacing);
        } else {
          Serial.println("Invalid Scan Request!");
        }
      }
      break;
      
    default:
      Serial.println("Unknown Char '" + String((char)Serial.read()) + "'!");
      break;
  };
}
