/**
 * 
 * Author: AintMina
 * Date: 18.3.2021
 * Version 0.1
 * 
 * Sketch to drive 2-joint robotic arm.
 * 
 */

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "angles.h"
#include "driver.h"
#include "RGBW.h"

#define SD_reader_pin 53

motor motor1(5, 2, 8, A8);
motor motor2(6, 3, 8, A9);

bool arm_down = true;
double theta1_old = 0, theta2_old = M_PI, r_old = 0, angle_old = 0, draw_speed = 1; 
int microsteps = 8;
byte mode, LED_mode, color, LED_counter, track_count;
bool did_the_thing, asked;
float LED_intensity = 1.0f;
int LED_speed = 500;
unsigned long time, time_old = 0;
String input, directory;
File root;
bool home_bool = false;
String track_temp;



void list_files(File dir, int num_tabs) {
    while(1) {
      File entry = dir.openNextFile();

      if (! entry) {
        break;
      }

      for (int i = 0; i < num_tabs; i++) {
        Serial3.write("\t");
      }

      if (!entry.isDirectory()) {
        if (strstr(entry.name(), ".THR")) {
          Serial3.write(entry.name());
          Serial3.write("\n");
        }
      }
      else if (entry.isDirectory()) {
        Serial3.write("/ ");
        Serial3.write(entry.name());
        Serial3.write("\n");
        list_files(entry, 2);
      }

      entry.close();
    }
  
}

bool check_files(File dir, String input) {
  while(1) {
    File entry = dir.openNextFile();
    String name = String(entry.name());

    if (name.equalsIgnoreCase(input)) {
      return true;
    }
    else if (entry.isDirectory()) {
      if (check_files(entry, input)) {
        directory = entry.name();
        return true;
      }
    }
    if (!entry) {
      return false;
    }
  }
}

void delete_line(File f, int line) {
  f.seek(0);

  for (int i = 0; i <= line; i++) {
    f.readStringUntil('\n');
  }

  f.seek(f.position()-4);
  f.println(" #");
}

void list_queue(File f) {
  f.seek(0);
  while (f.available()) {
    String line = f.readStringUntil('\n');
    char line_temp[line.length()];
    line.toCharArray(line_temp, sizeof(line_temp));
    Serial3.write(line_temp);
  }
}

void setup() {
  Serial3.begin(9600);
  Serial.begin(9600);
  delay(3000);
  Serial3.write("Send \"start\"\n");
  mode = 0;
  LED_mode = 0;
  leds_off();
  motor1.Stop();
  asked = false;
  color = 0;
  while(1) {
    if (Serial3.available()) {
      input = Serial3.readString();
      if (input.equalsIgnoreCase("start")) {
        break;
      }
    }
  }
  if (!SD.begin(SD_reader_pin)) {
    Serial3.write("Card failed\n");
    while(1) {
      if (SD.begin(SD_reader_pin)) {
        break;
      }
    }
  }
  Serial3.write("Card initialized.\n");
}

void loop() {

  // LEDs in fade mode
  if (LED_mode == 1) {
    time = millis();
    if (LED_counter > 225 * LED_intensity) {
      LED_counter = 0;
      color++;
      if (color > 4) {
        color = 1;
      }
    }
    did_the_thing = full_color_fade(time - time_old, color, LED_speed, LED_counter, LED_intensity);
    if (did_the_thing) {
      time_old = time;
      LED_counter++;
    }
  }
  else if (LED_mode == 2) {
    time = millis();
    if (LED_counter > 225 * LED_intensity) {
      LED_counter = 0;
      color++;
      if (color > 4) {
        color = 2;
      }
    }
    did_the_thing = fade_with_white(time - time_old, color, LED_speed, LED_counter, LED_intensity);
    if (did_the_thing) {
      time_old = time;
      LED_counter++;
    }
  }

  // Welcome screen
  if (mode == 0) {
    if (!asked) {
      Serial3.write("\n*** Welcome ***\n");
      Serial3.write("Commands:\n");
      Serial3.write("\"1\" start drawing.\n");
      Serial3.write("\"2\" start LEDs.\n");
      Serial3.write("\"3\" home.\n");
      Serial3.write("\"4\" become a millionaire.\n");
      asked = true;
    }
    if (Serial3.available()) {
      input = Serial3.readString();
      char input_temp[input.length() +1];
      input.toCharArray(input_temp, sizeof(input_temp));

      if (input.equalsIgnoreCase("1")) {
        mode = 1;
        asked = false;
      }
      else if (input.equalsIgnoreCase("2")) {
        Serial3.write("LED settings:\n");
        Serial3.write("\"L1\" to start full color fade. \n");
        Serial3.write("\"L1W\" to start full color fade with white. \n");
        Serial3.write("\"L2\" to choose a color.\n");
        Serial3.write("\"L3\" to stop LEDs.\n");
      }
      else if (input.equalsIgnoreCase("L1")) {
        Serial3.write("Full color fade.\n");
        Serial3.write("Choose speed(ms): \n");
        Serial3.write("eg. S 100\n");
        LED_mode = 1;
      }
      else if (input.equalsIgnoreCase("L1W")) {
        Serial3.write("Full color fade with white.\n");
        Serial3.write("Choose speed(ms): \n");
        Serial3.write("eg. S 100\n");
        LED_mode = 2;
      }
      else if (input_temp[0] == 'S' || input_temp[0] == 's') {
        strtok(input_temp, " ");
        char* speed = strtok(0, " ");
        LED_speed = atoi(speed);
        Serial3.write("LED speed set to: ");
        Serial3.write(speed);
        Serial3.write(" ms\nChoose intensity: 0 - 100 %\n");
        Serial3.write("eg. I 50\n");
      }
      else if (input_temp[0] == 'I' || input_temp[0] == 'i') {
        strtok(input_temp, " ");
        char* intens = strtok(0, " ");
        LED_intensity = atoi(intens) / 100.0;
        Serial3.write("LED intensity set to: ");
        Serial3.write(intens);
        Serial3.write(" %\n");
      }
      else if (input.equalsIgnoreCase("L2")) {
        Serial3.write("Choose color: R/G/B/W\n");
        Serial3.write("and choose intensity: 0 - 255\n");
        Serial3.write("eg. R 150\n");
      }
      else if (strcasestr(input_temp, "R") || strcasestr(input_temp, "G") || strcasestr(input_temp, "B") || strcasestr(input_temp, "W")) {
        char* COLOR = strtok(input_temp, " ");
        char* INTENSITY = strtok(0, " ");
        set_color(COLOR, atoi(INTENSITY));
        LED_mode = 0;
      }
      else if (input.equalsIgnoreCase("L3")) {
        leds_off();
        LED_mode = 0;
      }
      else if (input.equalsIgnoreCase("3")) {
        motor1.Enable();
        home(motor1, motor2);
        home_bool = true;
        r_old = 0;
        theta1_old = 0;
        theta2_old = M_PI;
        Serial3.write("Motors home.\n");
      }
      else if (input.equalsIgnoreCase("4")) {
        Serial3.write("Good try!");
      }
      else if (strstr(input_temp, "mot1")) {
        strtok(input_temp, " ");
        char* stp = strtok(0, " ");
        motor1.Enable();
        motor1.set_direction(atoi(stp));
        motor1.Step(atoi(stp)*microsteps, 3);
        Serial3.write("Done\n");
      }
      else if (strstr(input_temp, "mot2")) {
        strtok(input_temp, " ");
        char* stp = strtok(0, " ");
        motor2.Enable();
        motor2.set_direction(atoi(stp));
        motor2.Step(atoi(stp)*microsteps, 3);
        Serial3.write("Done\n");
      }
      else {
        Serial3.write("Invalid command!");
      }
    }
  }


  if (mode == 1) {

    if (!asked) {
      Serial3.write("Add tracks to queue!\n");
      Serial3.write("\"list\" to list tracks\n");
      Serial3.write("\"queue\" to list queue\n");
      asked = true;
      if (SD.exists("QUEUE.TXT")) {
        // Deletes the queue
        SD.remove("QUEUE.TXT");
      }
    }

    if (Serial3.available()) {
      input = Serial3.readString();
      if (input.equalsIgnoreCase("list")) {
        // Lists all the files
        root = SD.open("/");
        root.rewindDirectory();
        list_files(root, 0);
        root.rewindDirectory();
      }
      else if (input.equalsIgnoreCase("queue")) {
        File queue = SD.open("QUEUE.TXT");
        list_queue(queue);
        queue.close();
      }
      else if (input.equalsIgnoreCase("clear")) {
        if (SD.exists("QUEUE.TXT")) {
          // Deletes the queue
          SD.remove("QUEUE.TXT");
          Serial3.write("Queue cleared.");
        }
      }
      else if (input.equalsIgnoreCase("done")) {
        if (!SD.exists("QUEUE.TXT")) {
          Serial3.write("queue does not exist.\n");
          asked = false;
        }
        // Starts the queue
        else {
          asked = false;
          track_count = 0;
          root.close();
          mode = 2;
        }
      }
      else {
        // Adds files to the queue
        root.rewindDirectory();
        if (check_files(root, input)) {
          File file = SD.open("QUEUE.TXT", FILE_WRITE);
          file.print("/");
          file.print(directory);
          file.print("/");
          file.print(input);
          file.println("  ");
          char input_temp[input.length() + 1];
          input.toCharArray(input_temp, sizeof(input_temp));
          Serial3.write(input_temp);
          Serial3.write(" added to queue.\n");
          file.close();
        }
        else {
          Serial3.write("File does not exist.\n");
        }
      }
    }
  }

  if (mode == 2) {
    while (!home_bool) {
      if (!asked) {
        Serial3.write("Do you want to home?\n");
        Serial3.write("y/n\n");
        asked = true;
      }
      
      if (Serial3.available()) {
        input = Serial3.readString();
        if (input.equalsIgnoreCase("y")) {
          motor1.Enable();
          home(motor1, motor2);
          home_bool = true;
          r_old = 0;
          theta1_old = 0;
          theta2_old = M_PI;
          Serial3.write("Motors home.\n");
          asked = false;
        }
        else if (input.equalsIgnoreCase("n")) {
          home_bool = true;
          asked = false;
        }
        else {
          Serial3.write("Invalid command!");
        }

      }
    }

    File file = SD.open("QUEUE.TXT");
    while(1){
      track_temp = file.readStringUntil('\n');
      char track[track_temp.length()];
      track_temp.toCharArray(track, sizeof(track));
      if (!strstr(track, "#") && sizeof(track) > 5) {
        file.close();
        mode = 3;
        break;
      }
      else if (sizeof(track) <= 5) {
        Serial3.write("No more tracks queued.");
        file.close();
        mode = 0;
        asked = false;
        break;
      }
    }
  }

  if (mode == 3) {
    if (!asked) {
      Serial3.write("Starting to draw...\n");
      Serial3.write("Commands:\n");
      Serial3.write("\"1\" LED settings.\n");
      Serial3.write("\"2\" add files to queue.\n");
      Serial3.write("\"3\" list queue.\n");
      Serial3.write("\"4\" exit.\n\n");
      asked = true;
    }

    // Opens the track file
    char track[track_temp.length()-2];
    track_temp.toCharArray(track, sizeof(track));
    File file = SD.open(track);
    Serial3.write("Now drawing:\n");
    Serial3.write(track);
    Serial3.write("\n");
    motor1.Enable();

    if (file) {
      while (file.available() && mode == 3) {

        // Checking for input
        if (Serial3.available()) {
          input = Serial3.readString();
          char input_temp[input.length() +1];
          input.toCharArray(input_temp, sizeof(input_temp));

          if (input == "1") {
            Serial3.write("LED settings:\n");
            Serial3.write("\"L1\" to start full color fade.\n");
            Serial3.write("\"L1W\" to start full color fade with white. \n");
            Serial3.write("\"L2\" to choose a color.\n");
            Serial3.write("\"L3\" to stop LEDs.\n");
          }
          else if (input.equalsIgnoreCase("L1")) {
            Serial3.write("Full color fade\n");
            Serial3.write("Choose speed(ms): \n");
            Serial3.write("eg. S 100");
            LED_mode = 1;
          }
          else if (input.equalsIgnoreCase("L1W")) {
            Serial3.write("Full color fade with white.\n");
            Serial3.write("Choose speed(ms): \n");
            Serial3.write("eg. S 100\n");
            LED_mode = 2;
          }
          else if (input_temp[0] == 'S' || input_temp[0] == 's') {
            strtok(input_temp, " ");
            char* speed = strtok(0, " ");
            LED_speed = atoi(speed);
            Serial3.write("LED speed set to: ");
            Serial3.write(speed);
            Serial3.write("\nChoose intensity: 0 - 100 %\n");
            Serial3.write("eg. I 50\n");
          }
          else if (input_temp[0] == 'I' || input_temp[0] == 'i') {
            strtok(input_temp, " ");
            char* intens = strtok(0, " ");
            LED_intensity = atof(intens) / 100.0;
            Serial3.write("LED intensity set to :");
            Serial3.write(intens);
            Serial3.write(" %\n");
          }
          else if (input.equalsIgnoreCase("L2")) {
            Serial3.write("Choose color: R/G/B/W\n");
            Serial3.write("and choose intensity: 0 - 255\n");
            Serial3.write("eg. R 150\n");
          }
          else if (strcasestr(input_temp, "R") || strcasestr(input_temp, "G") || strcasestr(input_temp, "B") || strcasestr(input_temp, "W")) {
            char* COLOR = strtok(input_temp, " ");
            char* INTENSITY = strtok(0, " ");
            set_color(COLOR, atoi(INTENSITY));
            LED_mode = 0;
          }
          else if (input.equalsIgnoreCase("L3")) {
            leds_off();
            LED_mode = 0;
          }
          else if (input == "2") {
            Serial3.write("\"list\" to list the tracsks.\n");
            Serial3.write("\"clear\" to clear the queue.\n");
          }
          else if (input.equalsIgnoreCase("list")) {
            root = SD.open("/");
            root.rewindDirectory();
            list_files(root, 0);
            root.rewindDirectory();
            root.close();
          }
          else if (input.equalsIgnoreCase("clear")) {
            if (SD.exists("QUEUE.TXT")) {
              // Deletes the queue
              SD.remove("QUEUE.TXT");
              Serial3.write("Queue cleared.");
            }
          }
          else if (check_files(root, input)) {
            File queue = SD.open("QUEUE.TXT", FILE_WRITE);
            queue.print("/");
            queue.print(directory);
            queue.print("/");
            queue.print(input);
            queue.println("  ");
            char input_temp[input.length() +1];
            input.toCharArray(input_temp, sizeof(input_temp));
            Serial3.write(input_temp);
            Serial3.write(" added to queue.\n");
            queue.close();

            // TODO: add files to queue
          }
          else if (input == "3") {
            if (SD.exists("QUEUE.TXT")) {
              File queue = SD.open("QUEUE.TXT");
              list_queue(queue);
              queue.close();
            }
            else {
              Serial3.write("queue does not exist.");
            }
          }
          else if (input == "4" || input.equalsIgnoreCase("exit")) {
            mode = 0;
            break;
          }
          else {
            Serial3.write("Invalid command");
          }
        }

        // LEDs if its in fade mode
        if (LED_mode == 1) {
          time = millis();
          if (LED_counter > 225 * LED_intensity) {
            LED_counter = 0;
            color++;
            if (color > 4) {
              color = 1;
            }
          }
          did_the_thing = full_color_fade(time - time_old, color, LED_speed, LED_counter, LED_intensity);
          if (did_the_thing) {
            time_old = time;
            LED_counter++;
          }
        }
        else if (LED_mode == 2) {
          time = millis();
          if (LED_counter > 225 * LED_intensity) {
            LED_counter = 0;
            color++;
            if (color > 4) {
              color = 2;
            }
          }
          did_the_thing = fade_with_white(time - time_old, color, LED_speed, LED_counter, LED_intensity);
          if (did_the_thing) {
            time_old = time;
            LED_counter++;
          }
        }

        // Drawing the shit
        String line_temp = file.readStringUntil('\n');
        char line[line_temp.length() +1];
        line_temp.toCharArray(line, sizeof(line));

        if (!strstr(line, "#") && !strstr(line, "/")) {
          if (strlen(line) > 5) {
            char* angle = strtok(line, " ");
            char* r = strtok(0, " ");
            double angle_float = atof(angle);
            double r_float = atof(r);

            // Getting the angles for the arms
            double theta2 = polar_get_theta2(angle_float, r_float, arm_down);
            double theta1 = polar_get_theta1(abs(theta2), angle_float, r_float, arm_down);

            // Checking if the arm goes through origin
            arm_down = arm_down_check(r_float, r_old, arm_down);

            // Getting the change of angles
            double delta_theta1 = delta_angles(theta1, theta1_old);
            double delta_theta2 = delta_angles(theta2, theta2_old);

            // Getting the steps
            int step1 = steps(delta_theta1, microsteps);
            int step2 = steps(delta_theta2, microsteps);
            step2 += step1;

            // Setting the direction of the motors
            motor1.set_direction(step1);
            motor2.set_direction(-step2);

            draw_speed = r_float * 3;
            if (draw_speed < 1) {
              draw_speed = 1;
            }
            else if (draw_speed > 2) {
              draw_speed = 2;
            }

            // Moving the motors
            if ((abs(step1) > abs(step2)) && (step2 != 0)) {
              dual_steps(abs(step1), motor1, abs(step2), motor2, draw_speed);
            }
            else if ((abs(step1) < abs(step2)) && (step1 != 0)) {
              dual_steps(abs(step2), motor2, abs(step1), motor1, draw_speed);
            }
            else if (abs(step1) == abs(step2)) {
              equal_steps(abs(step1), motor2, motor1, draw_speed);
            }
            else if (step2 == 0 && step1 != 0) {
              motor1.Step(abs(step1), draw_speed);
            }
            else if (step1 == 0 && step2 != 0) {
              motor2.Step(abs(step2), draw_speed);
            }
            else {
              Serial3.write("Error in movement");
            }

            theta1_old = theta1_old + theta_from_steps(step1, microsteps);
            theta2_old = theta2_old + theta_from_steps(step2 - step1, microsteps);
            r_old = r_from_steps(theta1_old, theta2_old);

            if (theta1_old >= (2*M_PI)) {
              theta1_old -= (2*M_PI);
            }
            else if (theta1_old <= -(2*M_PI)) {
              theta1_old += (2*M_PI);
            }
          }
        }
      }

      file.close();

      File queue = SD.open("QUEUE.TXT",FILE_WRITE);
      delete_line(queue, track_count);
      queue.close();
      track_count++;
      mode = 2;
    }

    else {
      Serial3.write("Error opening the file\n");

      motor1.Stop();
      delay(5000);
    }
  }
}