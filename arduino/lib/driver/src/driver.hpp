/**
 * 
 Author: AintMina
 Date: 11.10.2022
 Version 0.1

 A driver for driving stepper motors for arduino with CNC-shield.

*/

#ifndef DRIVER_H
#define DRIVER_H

class motor {
    public:
    char dir, step, enable, sensor;
    motor(char dir_pin, char step_pin, char enable_pin, char sensor_pin);

    bool sensorCheck();
    void Stop();
    void Enable();
    void setDirection(int steps);
    void Step(int steps, float stepdelay = 1);
};

void dualSteps(int step1, motor motor1, int step2, motor motor2, float speed);
void equalSteps(int step, motor motor1, motor motor2, float speed);
void home(motor motor1, motor motor2);

#endif