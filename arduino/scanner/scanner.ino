#include <Wire.h>
#include "SparkFun_VL53L1X.h"

#include "stepper.h"

#define STEPPER_Z_DIRECTION_PIN 8
#define STEPPER_Z_STEP_PIN 9
#define STEPPER_Z_ENABLE_PIN 7

#define STEPPER_R_DIRECTION_PIN 12
#define STEPPER_R_STEP_PIN 11
#define STEPPER_R_ENABLE_PIN 10

#define LIMIT_TOP_PIN 2
#define LIMIT_BOTTOM_PIN 4

#define Z_TRAVEL_LIMIT 160.0f //mm

StepperMotor zMotor;
StepperMotor rMotor;
SFEVL53L1X distanceSensor;
uint32_t readyCode = 0;

float getScanDistance()
{
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
    setEnabled(&rMotor, true);
    rMotor.theta_degrees -= 360;
    moveMotorTo(&rMotor, 0, 500);

    for (uint32_t i = 0; i < num_points; i++)
    {
        delay(50);
        float dist = 0;
        for (int j = 0; j < 10; j++)
        {

            dist += (1.0 / 10.0) * (100.0 - getScanDistance());
        }
        delay(10);
        moveMotorRelative(&rMotor, spacing, 500);

        Serial.print('d');
        Serial.write((char *)&i, 4);
        Serial.write((char *)&dist, 4);
    }

    Serial.print('s');
}

void debugLog(String message)
{
    uint32_t message_length = (uint32_t)message.length();
    Serial.print('$');
    char s[4];
    memcpy(s, &message_length, 4);
    Serial.write(s, 4);
    Serial.print(message);
    Serial.flush();
}

inline bool isLimitPressed()
{
    return digitalRead(LIMIT_TOP_PIN) || digitalRead(LIMIT_BOTTOM_PIN);
}

void zeroZAxisUp()
{
    // Initialize the Z-axis motor
    setEnabled(&zMotor, true);
    moveMotorRelative(&zMotor, -360, 15);

    float current_delay = 150.0f;    // Current "speed" of the motor
    float acceleration = 0.075f;     // How much to change that speed by.
    float min_delay = 0.1f;         // Fastest we'll move the Z-motor

    // Set the direction to move up
    digitalWrite(zMotor.dir_pin, LOW);

    // Accelerate the motor towards the switch till impact.
    bool pressed = isLimitPressed();
    while (!pressed) {
        pulseMotor(&zMotor, current_delay);

        current_delay -= acceleration;
        if (current_delay < min_delay)
            current_delay = min_delay;

        pressed = isLimitPressed();
    }

    // Move the motor away from the switch
    moveMotorRelative(&zMotor, -1500, 15);
    delay(100);

    // Set the direction to move up
    digitalWrite(zMotor.dir_pin, LOW);
    current_delay = 100;

    pressed = isLimitPressed();
    while (!pressed) {
        pulseMotor(&zMotor, current_delay);
        pressed = isLimitPressed();
    }

    delay(100);
    zMotor.theta_degrees = Z_TRAVEL_LIMIT * 360;

    // Disable the motor at the home position.
    setEnabled(&zMotor, false);
}

// Function assumes that the axis is already zeroed.
float determinePartHeight()
{
    // Go to the top of the motion.
    setEnabled(&zMotor, true);
    moveMotorTo(&zMotor, Z_TRAVEL_LIMIT * 360, 15);

    // Decrement by 10mm till finding something solid.
    float distance = getScanDistance();
    while (distance > 200 && zMotor.theta_degrees > 0) {
        moveMotorRelative(&zMotor, -10 * 360.0f, 15);
        delay(250);
        distance = getScanDistance();
    }

    // setEnabled(&zMotor, false);
    return zMotor.theta_degrees / 360.0f;
}

void setup(void)
{
    Wire.begin();
    Serial.begin(9600);
    delay(1000);

    // Initialize the Motors
    initializeMotor(&zMotor, STEPPER_Z_STEP_PIN, STEPPER_Z_DIRECTION_PIN, STEPPER_Z_ENABLE_PIN);
    initializeMotor(&rMotor, STEPPER_R_STEP_PIN, STEPPER_R_DIRECTION_PIN, STEPPER_R_ENABLE_PIN);

    // Initialize other Digital IO
    pinMode(LIMIT_TOP_PIN, INPUT);
    pinMode(LIMIT_BOTTOM_PIN, INPUT);

    // Initialize the TOF sensor, and alert if failure.
    if (distanceSensor.begin())
        debugLog("TOF Sensor Failed!");

    debugLog("Scanner online!");
    readyCode = 1;
}

void loop(void)
{
    // debugLog(String(getScanDistance()));

    if (Serial.available() <= 0)
        return;

    switch (Serial.peek())
    {
    case 'r':
    {
        Serial.read();

        debugLog("Ready! RC=" + String(readyCode));
        Serial.print('r');
        Serial.write((char *)&readyCode, 4);
        Serial.flush();
        readyCode++;
        break;
    }

    case 'z':
    {
        if (Serial.available() >= 2)
        {
            Serial.read();

            char axis = Serial.read();
            if (axis == 'z')
            {
                debugLog("Zeroing Z...");
                // delay(2000);
                zeroZAxisUp();
                debugLog("Zeroed Z");

                Serial.print("zz");
                Serial.flush();
            }
            else if (axis == 'r')
            {
                debugLog("Zeroing R...");
                // delay(500);
                rMotor.theta_degrees = 0.0f;
                debugLog("Zeroed R");

                Serial.print("zr");
                Serial.flush();
            }
            else
            {
                debugLog("Unknown Axis Zeroing Requested!");

                Serial.print("z!");
                Serial.flush();
            }
        }
        break;
    }

    case 'h':
    {
        Serial.read();

        debugLog("Finding height...");
        // delay(1000);
        // float height = 101.14159f;
        float height = determinePartHeight();
        debugLog("Part height = " + String(height));

        Serial.print('h');
        Serial.write((char *)&height, 4);
        Serial.flush();
        break;
    }

    case 'g':
    {
        if (Serial.available() >= 6)
        {
            Serial.read();

            char axis = Serial.read();
            float pos = 0.0f;
            Serial.readBytes((uint8_t *)&pos, sizeof(float));

            if (axis == 'z')
            {
                debugLog("Moving Z to " + String(pos) + "...");
                // delay(2000);
                setEnabled(&zMotor, true);
                // moveMotorRelative(&zMotor, pos, 10);
                moveMotorTo(&zMotor, pos * 360, 10);
                setEnabled(&zMotor, false);
                debugLog("Move Z complete");

                Serial.print("g");
                Serial.flush();
            }
            else if (axis == 'r')
            {
                debugLog("Moving R to " + String(pos) + "...");
                // delay(2000);
                setEnabled(&rMotor, true);
                // moveMotorRelative(&rMotor, pos, 50);
                moveMotorTo(&rMotor, pos, 75);
                delay(100);
                setEnabled(&rMotor, false);
                debugLog("Move R complete");

                Serial.print("g");
                Serial.flush();
            }
            else
            {
                debugLog("Unknown Axis Move Requested!");
                Serial.print("q");
                Serial.flush();
            }
        }
        break;
    }

    case 's':
    {
        if (Serial.available() >= 9)
        {
            Serial.read();

            uint32_t num_points = 0;
            Serial.readBytes((uint8_t *)&num_points, sizeof(uint32_t));

            float spacing = 0.0f;
            Serial.readBytes((uint8_t *)&spacing, 4);

            if (num_points > 0)
            {
                debugLog("Scanning " + String(num_points) + " points, spacing = " + String(spacing));
                scanLine(num_points, spacing);
            }
            else
            {
                debugLog("Invalid Scan Request!");
                Serial.print("q");
                Serial.flush();
            }
        }
        break;
    }

    default:
    {
        debugLog("Unknown Arduino Char '" + String((char)Serial.read()) + "'!");
        break;
    }
    };
}
