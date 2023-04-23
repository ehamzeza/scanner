#pragma once

#include "Arduino.h"

const int STEPS_PER_REVOLUTION = 200;
const int MICROSTEPPING_SETTING = 32;

const float STEPS_PER_DEGREE = (STEPS_PER_REVOLUTION * MICROSTEPPING_SETTING) / 360.0f;

struct StepperMotor
{
    int step_pin, dir_pin, enable_pin;
    float theta_degrees;
};

void initializeMotor(StepperMotor* stepper, int step_pin, int dir_pin, int enable_pin);

void setEnabled(StepperMotor* stepper, bool enabled);

void moveMotorTo(StepperMotor* stepper, float theta_target, float delay_microsec);

void moveMotorRelative(StepperMotor* stepper, float theta_delta, float delay_microsec);

void pulseMotor(StepperMotor* stepper, float delay_microsec);
