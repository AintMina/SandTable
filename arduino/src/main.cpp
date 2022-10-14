#include "Arduino.h"
#include "driver.hpp"
#include "angles.hpp"
#include "leds.hpp"

int spr = 600;
int microstepping = 8;
float theta1_old = 0, theta2_old = M_PI, angle_old = 0;
bool inverted = false;

String led_track = "None";
unsigned long led_time = 0;
unsigned int led_speed = 100;
float led_intensity = 1.0;
float led_saturation = 0.0;
int draw_speed = 1;

motor motor1(6, 3, 8, A6);
motor motor2(7, 4, 8, A7);

led leds(11, 9, 10, 5);

void setup() {
    Serial.begin(500000);
    while(!Serial) {
        delay(20);
    }
    Serial.flush();
    motor1.Stop();
    Serial.write("ready\n");
    pinMode(13, OUTPUT);
}

void loop() {
    if(Serial.available() > 0) {
        String data_string = Serial.readStringUntil('\n');
        char data[data_string.length() +1];
        data_string.toCharArray(data, sizeof(data));
        
        // If input is a command
        if(data[0] == 'c' || data[0] == 'C') {
            strtok(data, " ");
            char* command = strtok(0, " ");
            String command_str = String(command);
            
            // Command to disable motors
            if(command_str.equalsIgnoreCase("stop")) {
                motor1.Stop();
            }
            // Homing command
            else if(command_str.equalsIgnoreCase("home")) {
                // Check that motors are enabled
                if(digitalRead(motor1.enable)) {
                  motor1.Enable();
                }
                home(motor1, motor2);
                theta1_old = 0.0;
                theta2_old = M_PI;
                Serial.write("0.0000 0.0000\n");
            }
          // Get command
          else if(command_str.equalsIgnoreCase("get")) {
            char* get_command = strtok(0, " ");
            String get = String(get_command);

            // Get coordinates command
            if(get.equalsIgnoreCase("coords")) {
              float angle, r;
              if(theta2_old == M_PI) {
                angle = theta1_old;
                r = 0;
              }
              else {
                angle = thetaFromArms(theta1_old, theta2_old);
                r = rFromArms(theta1_old, theta2_old);
              }
              String angle_str = String(angle, 5);
              String r_str = String(r, 5);
              String output_string = angle_str + " " + r_str + "\n";
              char output[output_string.length() +1];
              output_string.toCharArray(output, sizeof(output));
              Serial.write(output);
            }
          }
          // Set command
          else if(command_str.equalsIgnoreCase("set")) {
            char* set_command = strtok(0, " ");
            String set = String(set_command);

            // Set draw speed
            if(set.equalsIgnoreCase("speed")) {
                char* value = strtok(0, " ");
                draw_speed = atoi(value);
            }
            // Set led speed
            else if(set.equalsIgnoreCase("ledspeed")) {
                char* value = strtok(0, " ");
                led_speed = atoi(value);
            }
            // Set led intensity
            else if(set.equalsIgnoreCase("ledintensity")) {
                char* value = strtok(0, " ");
                int temp_value = atoi(value);
                led_intensity = float(temp_value) / 100.0;
            }
            // Set led saturation
            else if(set.equalsIgnoreCase("ledsaturation")) {
                char* value = strtok(0, " ");
                int temp_value = atoi(value);
                led_saturation = float(temp_value) / 100.0;
            }
          }
        }
        // Move motors separetly
        else if(data[0] == 'm' || data[0] == 'M') {
            strtok(data, " ");
            char* command = strtok(0, " ");
            int steps = atoi(command);

            if(data[1] == '1'){
                // Check that motors are enabled
                if(digitalRead(motor1.enable)) {
                  motor1.Enable();
                }
                motor1.setDirection(steps);
                motor1.Step(steps);
                theta1_old += thetaFromSteps(steps, microstepping);
                theta2_old += thetaFromSteps(-steps, microstepping);
            }
            else if(data[1] == '2'){
                // Check that motors are enabled
                if(digitalRead(motor1.enable)) {
                  motor1.Enable();
                }
                motor2.setDirection(steps);
                motor2.Step(steps);
                theta2_old += thetaFromSteps(steps, microstepping);
            }
        }
        // Led commands
        else if(data[0] == 'l' || data[0] == 'L') {
            strtok(data, " ");

            if(data[1] == 'c' || data[1] == 'C') {
                char* r = strtok(0, " ");
                char* g = strtok(0, " ");
                char* b = strtok(0, " ");
                char* w = strtok(0, " ");
                
                leds.setValue(atoi(r), atoi(g), atoi(b), atoi(w));
            }
            else if(data[1] == 't' || data[1] == 'T') {
                char* track = strtok(0, " ");
                led_track = String(track);
            }
        }
        else {
            // Check that motors are enabled
            if(digitalRead(motor1.enable)) {
              motor1.Enable();
            }
            // Splitting the input
            char* angle_char = strtok(data, " ");
            char* r_char = strtok(0, " ");
            float r = atof(r_char);
            float angle = 0;
            if(r == 0) {
              angle = angle_old;
            }
            else {
              angle = atof(angle_char);
            }

            // Checking if arm goes through origin
            float delta_angle = abs(angle - angle_old);
            while(delta_angle > 6) {
              delta_angle -= 3.14159265358979323846 * 2;
            }
            if(delta_angle < 3.2 && delta_angle > 3.1 && abs(r) < 0.03) {
              inverted != inverted;
            }

            // Getting the angles for the arms
            float theta2 = polarGetTheta2(angle, r);
            float theta1 = polarGetTheta1(theta2, angle, r, inverted, theta1_old);

            // Getting the change of angles
            float delta_theta1 = deltaAngles(theta1, theta1_old);
            float delta_theta2 = deltaAngles(theta2, theta2_old);

            // Getting the steps needed
            int step1 = steps(delta_theta1, microstepping);
            int step2 = steps(delta_theta2, microstepping);

            // Accounting the arm2 spin
            step2 += step1;

            // Setting the direction of the motors
            motor1.setDirection(step1);
            motor2.setDirection(step2);

            // Moving the motors
            if ((abs(step1) > abs(step2)) && (step2 != 0)) {
              dualSteps(abs(step1), motor1, abs(step2), motor2, draw_speed);
            }
            else if ((abs(step1) < abs(step2)) && (step1 != 0)) {
              dualSteps(abs(step2), motor2, abs(step1), motor1, draw_speed);
            }
            else if (abs(step1) == abs(step2)) {
              equalSteps(abs(step1), motor2, motor1, draw_speed);
            }
            else if (step2 == 0 && step1 != 0) {
              motor1.Step(abs(step1), draw_speed);
            }
            else if (step1 == 0 && step2 != 0) {
              motor2.Step(abs(step2), draw_speed);
            }
            else {
              Serial.write("Error in movement");
            }

            // Get arm angles from the steps
            theta1_old += thetaFromSteps(step1, microstepping);
            theta2_old += thetaFromSteps(step2 - step1, microstepping);

            // Check if angles are over 2PI
            if (theta1_old >= (2*M_PI)) {
              theta1_old -= (2*M_PI);
            }
            else if (theta1_old <= -(2*M_PI)) {
              theta1_old += (2*M_PI);
            }

            // Send the current position through serial
            angle = thetaFromArms(theta1_old, theta2_old);
            r = rFromArms(theta1_old, theta2_old);
            String angle_str = String(angle, 5);
            String r_str = String(r, 5);
            String output_string = angle_str + " " + r_str + "\n";
            char output[output_string.length() +1];
            output_string.toCharArray(output, sizeof(output));
            Serial.write(output);

            angle_old = angle;
        }
    }

    if(millis() - led_time > led_speed) {
        if(led_track.equalsIgnoreCase("colorfade")) {
            led_time = millis();
            leds.colorFade(led_intensity, led_saturation);
        }
    }
}