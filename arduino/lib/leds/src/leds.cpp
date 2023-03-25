/**
 * 
 Author: AintMina
 Date: 19.02.2023
 Version 1.5

 A driver for driving LED strip.

*/


#include "leds.hpp"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <math.h>

#define LOW 0
#define HIGH 1


/*
	Constructor for the LED object
*/
led::led(int r_p, int g_p, int b_p, int w_p) {
    counter = 0;
    color = 0;
    r_pin = r_p;
    g_pin = g_p;
    b_pin = b_p;
    w_pin = w_p;
	resolution = 12;
	cycle_wrap = pow(2,resolution) - 1;
	dir = 1;
	intensity = 1.0;
	saturation = 0.0;

    // Initiating the pins
    gpio_set_function(r_pin, GPIO_FUNC_PWM);
    r_num = pwm_gpio_to_slice_num(r_pin);
    pwm_set_wrap(r_num, cycle_wrap);
	pwm_set_enabled(r_num, true);

    gpio_set_function(g_pin, GPIO_FUNC_PWM);
    g_num = pwm_gpio_to_slice_num(g_pin);
    pwm_set_wrap(g_num, cycle_wrap);
	pwm_set_enabled(g_num, true);

    gpio_set_function(b_pin, GPIO_FUNC_PWM);
    b_num = pwm_gpio_to_slice_num(b_pin);
    pwm_set_wrap(b_num, cycle_wrap);
	pwm_set_enabled(b_num, true);

    gpio_set_function(w_pin, GPIO_FUNC_PWM);
    w_num = pwm_gpio_to_slice_num(w_pin);
    pwm_set_wrap(w_num, cycle_wrap);
	pwm_set_enabled(w_num, true);
}

/*
	Turns leds off
*/
void led::Off() {
	pwm_set_chan_level(r_num, PWM_CHAN_B, 0);
	pwm_set_chan_level(g_num, PWM_CHAN_B, 0);
	pwm_set_chan_level(b_num, PWM_CHAN_B, 0);
	pwm_set_chan_level(w_num, PWM_CHAN_B, 0);
}

/*
	Iterated the counter:
	directional = 1 for up and down
	directional = 0 for up then reset
*/
int led::SetCounter(int directional) {
	int interval = (cycle_wrap * intensity) / 255;
	if (dir) {
		counter += interval;
	}
	else {
		counter -= interval;
	}

	if (counter >= cycle_wrap * intensity) {
		if (directional) {
			counter = cycle_wrap * intensity;
			dir = 0;
			return 1;
		}
		else {
			counter = 0;
			return 1;
		}
	}
	else if (counter < 0) {
		if (directional) {
			counter = 0;
			dir = 1;
			return 1;
		}
		else {
			counter = 0;
			return 1;
		}
	}
	return 0;
}

/*
	Sets value for RGBW:
	0 = Red
	1 = Green
	2 = Blue
	3 = White
*/
void led::SetValue(int color, int value) {
	int rgbw[4] = {r_num, g_num, b_num, w_num};
	pwm_set_chan_level(rgbw[color], PWM_CHAN_B, value);
}

/*
	Sets values for RGBW
*/
void led::SetValues(int r, int g, int b, int w) {
	pwm_set_chan_level(r_num, PWM_CHAN_B, r);
	pwm_set_chan_level(g_num, PWM_CHAN_B, g);
	pwm_set_chan_level(b_num, PWM_CHAN_B, b);
	pwm_set_chan_level(w_num, PWM_CHAN_B, w);
}

/*
	Fades RGB
*/
void led::ColorFade() {
    int rgb[3] = {r_num, g_num, b_num};

    int last_color = color - 1;
    if (last_color == -1) {
        last_color = 2;
    }

	int unused_color = last_color - 1;
    if (unused_color == -1) {
        unused_color = 2;
    }

    pwm_set_chan_level(w_num, PWM_CHAN_B, int(cycle_wrap * saturation * intensity));
    pwm_set_chan_level(rgb[color], PWM_CHAN_B, int(counter));
    pwm_set_chan_level(rgb[last_color], PWM_CHAN_B, int((cycle_wrap * intensity) - counter));
    pwm_set_chan_level(rgb[unused_color], PWM_CHAN_B, 0);

    if (SetCounter(0)) {
        color++;

        if (color > 2) {
            color = 0;
        }
    }
}

/*
	Pulses one color:
	0 = Red
	1 = Green
	2 = Blue
	3 = White
*/
void led::OneColorFade(int color) {
    int rgbw[4] = {r_num, g_num, b_num, w_num};

	for (int i = 0; i < 4; i++) {
		if (i == color) {
			pwm_set_chan_level(rgbw[i], PWM_CHAN_B, counter);
		}
		else {
			pwm_set_chan_level(rgbw[i], PWM_CHAN_B, 0);
		}
	}

	SetCounter(1);
}