/**
 * 
 Author: AintMina
 Date: 11.10.2022
 Version 0.1

 A driver for driving LED strip.

*/

#ifndef LEDS_H
#define LEDS_H

class led {
    public:
    int r_pin, g_pin, b_pin, w_pin;
    int counter, color;
    led(int r_p, int g_p, int b_p, int w_p);

    void Off();
    void setValue(int r, int g, int b, int w);
    void colorFade();
    void allFade();

};

#endif