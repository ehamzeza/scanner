#include <Wire.h>
#include "SparkFun_VL53L1X.h"

SFEVL53L1X distanceSensor;

uint32_t readyCode = 0;

float getScanDistance() {
  distanceSensor.startRanging();
  while (!distanceSensor.checkForDataReady())
    delay(1);
  
  int distance = distanceSensor.getDistance(); // Measured in mm
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  return distance;
}

void scanLine(uint32_t num_points, float spacing)
{
  for (uint32_t i = 0; i < num_points; i++) {
//    delay(250);
    float dist = getScanDistance();

    Serial.print('d');
    Serial.write((char*) &i, 4);
    Serial.write((char*) &dist, 4);
  }

  Serial.print('s');
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
  delay(1000);

  if (distanceSensor.begin()) {
    debugLog("TOF Sensor Failed!");
  }

  debugLog("Scanner online!");
  readyCode = 1;
}

void loop(void)
{
  if (Serial.available() <= 0)
    return;
  
  switch (Serial.peek()) {
    case 'r': {
      Serial.read();
      
      debugLog("Ready! RC=" + String(readyCode));
      Serial.print('r');
      Serial.write((char*) &readyCode, 4);
      Serial.flush();
      readyCode ++;
      break;
    }

    case 'z': {
      if (Serial.available() >= 2) {
        Serial.read();
        
        char axis = Serial.read();
        if (axis == 'z') {
          debugLog("Zeroing Z...");
          delay(2000);
          debugLog("Zeroed Z");
          
          Serial.print("zz");
          Serial.flush();
          
        } else if (axis == 'r') {
          debugLog("Zeroing R...");
          delay(500);
          debugLog("Zeroed R");
          
          Serial.print("zr");
          Serial.flush();
          
        } else {
          debugLog("Unknown Axis Zeroing Requested!");

          Serial.print("z!");
          Serial.flush();
        }
      }
      break;
    }
    
    case 'h': {
      Serial.read();
      float height = 101.14159f;
      
      debugLog("Finding height...");
      delay(1000);
      debugLog("Part height = " + String(height));
      
      Serial.print('h');
      Serial.write((char*) &height, 4);
      Serial.flush();
      break;
    }

    case 'g': {
      if (Serial.available() >= 6) {
        Serial.read();

        char axis = Serial.read();
        float pos = 0.0f;
        Serial.readBytes((uint8_t*) &pos, sizeof(float));

        
        if (axis == 'z') {
          debugLog("Moving Z to " + String(pos) + "...");
          delay(2000);
          debugLog("Move Z complete");
          
          Serial.print("g");
          Serial.flush();
          
        } else if (axis == 'r') {
          debugLog("Moving R to " + String(pos) + "...");
          delay(2000);
          debugLog("Move R complete");
          
          Serial.print("g");
          Serial.flush();
          
        } else {
          debugLog("Unknown Axis Move Requested!");
          Serial.print("q");
          Serial.flush();
        }
      }
      break;
    }

    case 's': {
      if (Serial.available() >= 9) {
        Serial.read();
        
        uint32_t num_points = 0;
        Serial.readBytes((uint8_t*) &num_points, sizeof(uint32_t));
        
        float spacing = 0.0f;
        Serial.readBytes((uint8_t*) &spacing, 4);
        
        if (num_points > 0) {
          debugLog("Scanning " + String(num_points) + " points, spacing = " + String(spacing));
          scanLine(num_points, spacing);
        } else {
          debugLog("Invalid Scan Request!");
          Serial.print("q");
          Serial.flush();
        }
      }
      break;
    }
      
    default: {
      debugLog("Unknown Arduino Char '" + String((char)Serial.read()) + "'!");
      break;
    }
  };
}
