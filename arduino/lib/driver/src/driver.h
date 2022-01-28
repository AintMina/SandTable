/**
 * 
 Author: AintMina
 Date: 27.3.2021
 Version 0.1

 A driver for driving stepper motors for arduino with CNC-shield.

*/


class motor {
    public:
    char dir, step, enable, sensor;

    // Constructor for the pins
    motor(char dir_pin, char step_pin, char enable_pin, char sensor_pin) {
        dir = dir_pin;
        enable = enable_pin;
        step = step_pin;
        sensor = sensor_pin;

        // Initiating the pins
        pinMode(dir, OUTPUT);
        pinMode(enable, OUTPUT);
        pinMode(step, OUTPUT);
        pinMode(sensor, INPUT);
    }

    bool sensorCheck() {
        int val = analogRead(sensor);
        if (val <= 200) {
            return false;
        }
        else if (val > 200) {
            return true;
        }
    }

    // Disable the motor
    void Stop() {
        digitalWrite(enable, HIGH);
    }

    // Enable the motor
    void Enable() {
        digitalWrite(enable, LOW);
        delay(10);
    }

    // Sets the direction pin and enables the motor
    void setDirection(int steps) {
        if (steps >= 0) {
            digitalWrite(dir, HIGH);
        }
        else if (steps < 0) {
            digitalWrite(dir, LOW);
        }
    }

    void Step(int steps, float stepdelay = 1) {
        for (int i = 0; i < steps; i++) {
            digitalWrite(step, HIGH);
            delay(stepdelay);
            digitalWrite(step, LOW);
            delay(stepdelay);
        }
    }
};

// Function to divide the steps
// motor1 needs to have more steps!
void dualSteps(int step1, motor motor1, int step2, motor motor2, float speed) {
    int v = step1 / step2;      // Variable to figure out step ratio
    int counter = 0;
    int step2_counter = 0;

    for (int i = 0; i < step1; i++) {
        motor1.Step(1, speed);
        counter++;
        if ((counter == v) && (step2_counter < step2)) {
            motor2.Step(1, speed);
            step2_counter++;
            counter = 0;
        }
    }

    // Checking if step2 did all the steps
    while (step2_counter < step2) {
        motor2.Step(1, speed);
        step2_counter++;
    }
}

void equalSteps(int step, motor motor1, motor motor2, float speed) {
    for (int i = 0; i < step; i++) {
        motor1.Step(1, speed);
        motor2.Step(1, speed);
    }
}

void home(motor motor1, motor motor2) {
    motor1.setDirection(1);
    while (!(motor1.sensorCheck())) {
        motor1.Step(1);
    }

    motor2.setDirection(1);
    while (!(motor2.sensorCheck())) {
        motor2.Step(1);
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