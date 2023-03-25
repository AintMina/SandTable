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
	int r_num, g_num, b_num, w_num;
    int counter, color, resolution, cycle_wrap, dir;
	float intensity, saturation;
    led(int r_p, int g_p, int b_p, int w_p);

    void Off();
	int SetCounter(int directional);
	void SetValue(int color, int value);
    void SetValues(int r, int g, int b, int w);
    void ColorFade();
	void OneColorFade(int color);

};

#endif