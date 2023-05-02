// Pico-SDK specific libraries
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/time.h>
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// C libraries
#include <stdio.h>
#include <cmath>

// Local libraries
#include "leds.hpp"
#include "driver.hpp"
#include "angles.hpp"


// Communication
#define BUFFER_LENGTH 10
/*
	Commands:
	1st bit = command
	2nd bit = data size
	rest n bits = data
*/
#define COMMAND_STOP 				0x00		// Returns: Nothing
#define COMMAND_ANGLE 				0x01		// Returns: Nothing
#define COMMAND_R					0x02		// Returns: Actual angle and r (floats as char*)
#define COMMAND_X_COORDINATE		0x03		// Returns: Nothing
#define COMMAND_Y_COORDINATE		0x04		// Returns: ---
#define COMMAND_HOME				0x05		// Returns: Nothing
#define COMMAND_UPDATE_ANGLE		0x06		// Returns: Nothing
#define COMMAND_UPDATE_R			0x07		// Returns: Nothing
#define COMMAND_M1_STEP				0x08		// Returns: Actual angle and r (floats as char*)
#define COMMAND_M2_STEP				0x09		// Returns: Actual angle and r (floats as char*)
#define COMMAND_SET_MOTOR_SPEED		0x0a		// Returns: Nothing
#define COMMAND_SET_LED_TRACK		0x0b		// Returns: Nothing
#define COMMAND_SET_LED_SPEED		0x0c		// Returns: Nothing
#define COMMAND_SET_LED_INTENSITY	0x0d		// Returns: Nothing
#define COMMAND_SET_LED_SATURATION	0x0e		// Returns: Nothing
#define COMMAND_SET_LED_R			0x0f		// Returns: Nothing
#define COMMAND_SET_LED_G			0x10		// Returns: Nothing
#define COMMAND_SET_LED_B			0x11		// Returns: Nothing
#define COMMAND_SET_LED_W			0x12		// Returns: Nothing
#define COMMAND_GET_ANGLE			0x13		// Returns: Angle (float as char*)
#define COMMAND_GET_R				0x14		// Returns: R (float as char*)
#define COMMAND_RESET				0x15		// Returns: Nothing


// LED PINS
#define R_PIN 25		// PWM4 B - D2
#define G_PIN 7			// PWM3 B - D10
#define B_PIN 21		// PWM2 B - D9
#define W_PIN 17		// PWM0 B - D5

led leds(R_PIN, G_PIN, B_PIN, W_PIN);


// Motors
#define MOTORS_ENABLE 20	// D8
#define M1_DIR 18			// D6
#define M1_STEP 15			// D3
#define M1_SENSOR 6 		// D13
#define M2_DIR 19			// D7
#define M2_STEP 16			// D4
#define M2_SENSOR 4 		// D12

motor motor1(M1_DIR, M1_STEP, MOTORS_ENABLE, M1_SENSOR);
motor motor2(M2_DIR, M2_STEP, MOTORS_ENABLE, M2_SENSOR);


// LED settings
int led_speed = 50000;
absolute_time_t  led_time = get_absolute_time();
float led_intensity = 0.4;
float led_saturation = 0.0;
/*
	LED tracks:
	0 = static color
	1 = red pulse
	2 = green pulse
	3 = blue pulse
	4 = white pulse
	5 = RGB fade
*/
int led_track = 0;


// Motor settings
int spr = 600;
int microstepping = 8;
float theta1_old = 0;
float theta2_old = M_PI;
float angle_old = 0;
float r_old = 0;
bool inverted = false;
int draw_speed = 1;


/*
	Second core function
*/
void second_core() {
	while (1) {
		float angle = 0.0;
		float r = 0.0;
		int fifo = 0;
		
		fifo = multicore_fifo_pop_blocking();
		motor1.Enable();

		// Home command
		if (fifo == COMMAND_HOME) {
			home(motor1, motor2);
			theta1_old = 0.0;
			theta2_old = M_PI;
			angle_old = 0;
			r_old = 0;
			multicore_fifo_push_blocking(0);
			continue;
		}

		// M1 steps
		else if (fifo == COMMAND_M1_STEP) {
			int steps = multicore_fifo_pop_blocking();
			motor1.setDirection(steps);
			motor1.Step(abs(steps), 5);
			multicore_fifo_push_blocking(0);
			continue;
		}

		// M2 steps
		else if (fifo == COMMAND_M2_STEP) {
			int steps = multicore_fifo_pop_blocking();
			motor2.setDirection(steps);
			motor2.Step(abs(steps), 5);
			multicore_fifo_push_blocking(0);
			continue;
		}

		// Theta nd R
		else {
			fifo = multicore_fifo_pop_blocking();
			angle = *reinterpret_cast<float*>(&fifo);
			fifo = multicore_fifo_pop_blocking();
			r = *reinterpret_cast<float*>(&fifo);
		}

		// If r is zero ignore angle
		if (r == 0) {
			angle = angle_old;
		}
		// If r is too much - ignore
		if (r > 1.0) {
			multicore_fifo_push_blocking(1);
			continue;
		}
		// If delta r is too much - ignore
		else if (abs(r - r_old) > 0.05) {
			multicore_fifo_push_blocking(1);
			continue;
		}

		// Getting angles for the arms
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


		angle_old = thetaFromArms(theta1_old, theta2_old);
		r_old = rFromArms(theta1_old, theta2_old);
		
		multicore_fifo_push_blocking(1);

	}
}


/*
	Read serial input
*/
uint16_t read_serial(uint8_t *buffer) {
	uint16_t buffer_index = 0;
	while (1) {
		int c = getchar_timeout_us(100);
		if (c != PICO_ERROR_TIMEOUT &&  buffer_index < BUFFER_LENGTH) {
			buffer[buffer_index++] = c;
		}
		else {
			break;
		}
	}
	return buffer_index;
}


/*
	Combine n number of bytes to float
*/
float combine_float_bytes(uint8_t *bytes) {
	uint32_t value = 0;
	int n = bytes[0];

	for (int i = 1; i <= n; i++) {
		value |= bytes[i] << (8 * (i - 1));
	}

    return *reinterpret_cast<float*>(&value);
}


/*
	Combine n number of bytes to int
*/
int combine_int_bytes(uint8_t *bytes) {
	int result = 0;
	int n = bytes[0];

	for (int i = 1; i <= n; i++) {
		result |= static_cast<int32_t>(bytes[i]) << ((8 * (i - 1)));
	}
	return result;
}


/*
	Splits float into bytes for serial
*/
void split_float_to_bytes(float value, unsigned char* bytes) {
	unsigned char* int_bytes = reinterpret_cast<unsigned char*>(&value);

	for (int i = 0; i < 4; i++) {
		bytes[i] = *(int_bytes + i);
	}
}


int main() {
	// Initialize serial
    stdio_init_all();

	// Loop while serial not connected
	while(!stdio_usb_connected()) {
		leds.OneColorFade(0);
		sleep_us(led_speed);
	}

	// Turn off LEDs and reset the counter
	leds.Off();
	leds.counter = 0;

	// Launch the second core
    multicore_launch_core1(second_core);

	// Main function variables
	float angle = 0.0;
	float r = 0.0;
	float x = 0.0;
	float y = 0.0;

    while (1) {

		// Buffer for serial commands
		uint8_t buf[BUFFER_LENGTH] = {0xfe};
		read_serial(&buf[0]);

		// Commands
		switch (buf[0]) {
			// Stop
			case COMMAND_STOP: {
				motor1.Stop();
				break;
			}

			// Angle first
			case COMMAND_ANGLE: {
				angle = combine_float_bytes(&buf[1]);
				break;
			}

			// R second
			case COMMAND_R: {
				r = combine_float_bytes(&buf[1]);
				// TODO: safeguard to check if error in data
				multicore_fifo_push_blocking(COMMAND_R);
				multicore_fifo_push_blocking(*reinterpret_cast<uint32_t*>(&angle));
				multicore_fifo_push_blocking(*reinterpret_cast<uint32_t*>(&r));
				break;
			}

			// X coordnate first
			case COMMAND_X_COORDINATE: {
				x = combine_float_bytes(&buf[1]);
				break;
			}

			// Y coordnate second
			case COMMAND_Y_COORDINATE: {
				y = combine_float_bytes(&buf[1]);
				// TODO: Convert cartesian to polar...
				break;
			}

			// Home
			case COMMAND_HOME: {
				multicore_fifo_push_blocking(COMMAND_HOME);
				break;
			}

			// Update angle first
			case COMMAND_UPDATE_ANGLE: {
				angle_old = combine_float_bytes(&buf[1]);
				break;
			}

			// Update r second
			case COMMAND_UPDATE_R: {
				r_old = combine_float_bytes(&buf[1]);
                theta2_old = polarGetTheta2(angle_old, r_old);
                theta1_old = polarGetTheta1(theta2_old, angle_old, r_old, inverted, angle_old);
				break;
			}

			// M1 step
			case COMMAND_M1_STEP: {
				multicore_fifo_push_blocking(COMMAND_M1_STEP);
				int steps = combine_int_bytes(&buf[1]);
				multicore_fifo_push_blocking(steps);
				break;
			}

			// M2 step
			case COMMAND_M2_STEP: {
				multicore_fifo_push_blocking(COMMAND_M2_STEP);
				int steps = combine_int_bytes(&buf[1]);
				multicore_fifo_push_blocking(steps);
				break;
			}

			// Set motor speed
			case COMMAND_SET_MOTOR_SPEED: {
				draw_speed = combine_int_bytes(&buf[1]);
				break;
			}

			// Set led track
			case COMMAND_SET_LED_TRACK: {
				led_track = combine_int_bytes(&buf[1]);
				break;
			}

			// Set led speed
			case COMMAND_SET_LED_SPEED: {
				led_speed = combine_int_bytes(&buf[1]) * 1000;
				break;
			}

			// Set led intensity
			case COMMAND_SET_LED_INTENSITY: {
				float value = combine_float_bytes(&buf[1]);

				if (value > 1.0) {
					value = 1.0;
				}
				else if (value <= 0.0) {
					value = 0.00001;
				}

				leds.intensity = value;
				break;
			}

			// Set led saturation
			case COMMAND_SET_LED_SATURATION: {
				float value = combine_float_bytes(&buf[1]);

				if (value > 1.0) {
					value = 1.0;
				}
				else if (value <= 0.0) {
					value = 0.0;
				}

				leds.saturation = value;
				break;
			}

			// Set LED R
			case COMMAND_SET_LED_R: {
				led_track = 0;
				float value = (float)(combine_int_bytes(&buf[1]));

				if (value > 255.0) {
					value = 255.0;
				}
				else if (value <= 0.0) {
					value = 0.0;
				}

				value /= 255.0;
				value *= leds.cycle_wrap;
				leds.SetValue(0, (int)(value));
				break;
			}

			// Set LED G
			case COMMAND_SET_LED_G: {
				led_track = 0;
				float value = (float)(combine_int_bytes(&buf[1]));

				if (value > 255.0) {
					value = 255.0;
				}
				else if (value <= 0.0) {
					value = 0.0;
				}

				value /= 255.0;
				value *= leds.cycle_wrap;
				leds.SetValue(1, (int)(value));
				break;
			}

			// Set LED B
			case COMMAND_SET_LED_B: {
				led_track = 0;
				float value = (float)(combine_int_bytes(&buf[1]));

				if (value > 255.0) {
					value = 255.0;
				}
				else if (value <= 0.0) {
					value = 0.0;
				}

				value /= 255.0;
				value *= leds.cycle_wrap;
				leds.SetValue(2, (int)(value));
				break;
			}

			// Set LED W
			case COMMAND_SET_LED_W: {
				led_track = 0;
				float value = (float)(combine_int_bytes(&buf[1]));

				if (value > 255.0) {
					value = 255.0;
				}
				else if (value <= 0.0) {
					value = 0.0;
				}

				value /= 255.0;
				value *= leds.cycle_wrap;
				leds.SetValue(3, (int)(value));
				break;
			}

			// Get Angle
			case COMMAND_GET_ANGLE: {
				printf("%f\n", angle_old);
			}

			// Get R
			case COMMAND_GET_R: {
				printf("%f\n", r_old);
			}

			// Reset core 1
			case COMMAND_RESET: {
				multicore_reset_core1();
    			multicore_launch_core1(second_core);
			}
		}

		// Listening to core 1
		uint32_t core1_msg = 0;
		if (multicore_fifo_rvalid()) {
			multicore_fifo_pop_timeout_us(100, &core1_msg);
		}

		// Returning the angle and r when motors are done
		if (core1_msg == 1) {
			printf("%f\n", angle_old);
			fflush(stdout);
			printf("%f\n", r_old);
			fflush(stdout);
		}

		// Trigger the LED track
		if(absolute_time_diff_us(led_time, get_absolute_time()) > led_speed) {
			led_time = get_absolute_time();

			if (led_track == 1) {
				leds.OneColorFade(0);
			}

			else if (led_track == 2) {
				leds.OneColorFade(1);
			}

			else if (led_track == 3) {
				leds.OneColorFade(2);
			}

			else if (led_track == 4) {
				leds.OneColorFade(3);
			}

			else if (led_track == 5) {
				leds.ColorFade();
			}
		}

    }
}