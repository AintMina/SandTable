/**
 * 
 Author: AintMina
 Date: 30.01.2022
 Version 0.1

 A driver for driving LED strip.

*/


#include "leds.hpp"
#include "Arduino.h"

led::led(int r_p, int g_p, int b_p, int w_p) {
    counter = 0;
    color = 0;
    r_pin = r_p;
    g_pin = g_p;
    b_pin = b_p;
    w_pin = w_p;

    // Initiating the pins
    pinMode(r_pin, OUTPUT);
    pinMode(g_pin, OUTPUT);
    pinMode(b_pin, OUTPUT);
    pinMode(w_pin, OUTPUT);
}

void led::Off() {
    digitalWrite(r_pin, LOW);
    digitalWrite(g_pin, LOW);
    digitalWrite(b_pin, LOW);
    digitalWrite(w_pin, LOW);
}

void led::setValue(int r, int g, int b, int w) {
    analogWrite(r_pin, r);
    analogWrite(g_pin, g);
    analogWrite(b_pin, b);
    analogWrite(w_pin, w);
}

void led::colorFade(float intensity, float saturation) {
    int rgb[3] = {r_pin, g_pin, b_pin};
    int last_color = color - 1;
    if (last_color == -1) {
        last_color = 2;
    }

    analogWrite(w_pin, int(255 * saturation * intensity));
    analogWrite(rgb[color], int(counter * intensity));
    analogWrite(rgb[last_color], int((255 - counter) * intensity));
    counter++;

    if (counter > 255) {
        counter = 0;
        color++;

        if (color > 2) {
            color = 0;
        }
    }
}

void led::allFade() {
    int rgbw[4] = {r_pin, g_pin, b_pin, w_pin};
    int last_color = color - 1;
    if (last_color == -1) {
        last_color = 3;
    }

    analogWrite(rgbw[color], counter);
    analogWrite(rgbw[last_color], 255 - counter);
    counter++;

    if (counter > 255) {
        counter = 0;
        color++;

        if (color > 3) {
            color = 0;
        }
    }
    
}