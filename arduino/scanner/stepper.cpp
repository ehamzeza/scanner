#include "stepper.h"

void initializeMotor(StepperMotor* stepper, int step_pin, int dir_pin, int enable_pin)
{
    stepper->step_pin = step_pin;
    stepper->dir_pin = dir_pin;
    stepper->enable_pin = enable_pin;

    // Initial software zero
    stepper->theta_degrees = 0.0f;

    // Setup the arduino pin settings
    pinMode(stepper->step_pin, OUTPUT);
    pinMode(stepper->dir_pin, OUTPUT);
    pinMode(stepper->enable_pin, OUTPUT);

    // Start with the motor disabled
    setEnabled(stepper, false);
}

void setEnabled(StepperMotor* stepper, bool enabled)
{
    // Digital LOW enables the stepper driver.
    digitalWrite(stepper->enable_pin, enabled ? LOW : HIGH);
}

void moveMotorTo(StepperMotor* stepper, float theta_target, float delay_microsec)
{
    moveMotorRelative(stepper, theta_target - stepper->theta_degrees, delay_microsec);
}

void moveMotorRelative(StepperMotor* stepper, float theta_delta, float delay_microsec)
{
    // Turn on the motor and set the direction.
    // setEnabled(stepper, true);
    digitalWrite(stepper->dir_pin, theta_delta < 0 ? HIGH : LOW);

    // Compute the required number of steps and pulse them.
    unsigned long required_steps = abs(theta_delta) * STEPS_PER_DEGREE;
    for (unsigned long i = 0; i < required_steps; i++) {
        digitalWrite(stepper->step_pin, HIGH);
        delayMicroseconds(delay_microsec);
        digitalWrite(stepper->step_pin, LOW);
        delayMicroseconds(delay_microsec);
    }

    stepper->theta_degrees += theta_delta;

    // Pausing for a moment with the drive enabled ensures positional accuracy
    // from any inertail forces.
    // delay(50);

    // // Turn off the motor.
    // setEnabled(stepper, false);
}

void pulseMotor(StepperMotor* stepper, float delay_microsec)
{
    digitalWrite(stepper->step_pin, HIGH);
    delayMicroseconds(delay_microsec);
    digitalWrite(stepper->step_pin, LOW);
    delayMicroseconds(delay_microsec);
}