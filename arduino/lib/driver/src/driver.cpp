/**
 * 
 Author: AintMina
 Date: 21.02.2023
 Version 1.5

 A driver for driving stepper motors for arduino with CNC-shield.

*/

#include "driver.hpp"
#include "pico/stdlib.h"
#include <cmath>


/*
	Constructor for the motor object
*/
motor::motor(char dir_pin, char step_pin, char enable_pin, char sensor_pin) {
    dir = dir_pin;
    enable = enable_pin;
    step = step_pin;
    sensor = sensor_pin;

    // Initiating the pins
    gpio_init(dir);
    gpio_init(enable);
    gpio_init(step);
    gpio_init(sensor);
    gpio_set_dir(dir, GPIO_OUT);
    gpio_set_dir(enable, GPIO_OUT);
    gpio_set_dir(step, GPIO_OUT);
    gpio_set_dir(sensor, GPIO_IN);
}


/*
	Checks the input from the sensor
*/
bool motor::sensorCheck() {
    int val = gpio_get(sensor);
    if (val) {
        return true;
    }
    else if (!val) {
        return false;
    }
}


/*
	Disables the motor
*/
void motor::Stop() {
    gpio_put(enable, 1);
}


/*
	Enable the motor
*/
void motor::Enable() {
    gpio_put(enable, 0);
}


/*
	Sets the direction pin and enables the motor
*/
void motor::setDirection(int steps) {
    if (steps >= 0) {
        gpio_put(dir, 1);
    }
    else if (steps < 0) {
        gpio_put(dir, 0);
    }
}


/*
	Step function
*/
void motor::Step(int steps, float stepdelay) {
    for (int i = 0; i < abs(steps); i++) {
        gpio_put(step, 1);
        sleep_ms(stepdelay);
        gpio_put(step, 0);
        sleep_ms(stepdelay);
    }
}


/*
	Function to divide the steps
	motor1 needs to have more steps!
*/
void dualSteps(int step1, motor motor1, int step2, motor motor2, float speed) {
    int v = step1 / step2;      // Variable to figure out step ratio
    float v_f = float(step1) / float(step2);
    v_f -= float(v);            // Variable to figure out the remainder of step ratio
    int counter = 0;
    int step2_counter = 0;
    float extra_counter = 0.0;

    for (int i = 0; i < step1; i++) {
        motor1.Step(1, speed);
        counter++;
        if ((counter >= v) && (step2_counter < step2)) {
            if (extra_counter > 1.0) {
                extra_counter -= v;
                continue;
            }
            motor2.Step(1, speed);
            step2_counter++;
            counter = 0;
            extra_counter += v_f;
        }
    }

    // Checking if step2 did all the steps
    while (step2_counter < step2) {
        motor2.Step(1, speed);
        step2_counter++;
    }
}


/*
	Moves both of the motors at the same time
*/
void equalSteps(int step, motor motor1, motor motor2, float speed) {
    for (int i = 0; i < step; i++) {
        motor1.Step(1, speed);
        motor2.Step(1, speed);
    }
}


/*
	Moves the motors in order until signal from sensor
*/
void home(motor motor1, motor motor2) {
    motor1.setDirection(1);
    while (!(motor1.sensorCheck())) {
        motor1.Step(1, 1);
    }

    motor2.setDirection(1);
    while (!(motor2.sensorCheck())) {
        motor2.Step(1, 1);
    }

    motor1.setDirection(-1);
    motor1.Step(100, 5);

    motor1.setDirection(1);
    while (!(motor1.sensorCheck())) {
        motor1.Step(1, 10);
    }

    motor2.setDirection(-1);
    motor2.Step(100, 5);

    motor2.setDirection(1);
    while (!(motor2.sensorCheck())) {
        motor2.Step(1, 10);
    }
}