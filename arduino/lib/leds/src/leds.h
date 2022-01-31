/**
 * 
 Author: AintMina
 Date: 30.01.2022
 Version 0.1

 A driver for driving LED strip.

*/


class led {
    public:
    int r_pin, g_pin, b_pin, w_pin;
    int counter, color;


    // Constructor for the pins
    led(int r_p, int g_p, int b_p, int w_p) {
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

    void Off() {
        digitalWrite(r_pin, LOW);
        digitalWrite(g_pin, LOW);
        digitalWrite(b_pin, LOW);
        digitalWrite(w_pin, LOW);
    }

    void setValue(int r, int g, int b, int w) {
        analogWrite(r_pin, r);
        analogWrite(g_pin, g);
        analogWrite(b_pin, b);
        analogWrite(w_pin, w);
    }

    void colorFade() {
        int rgb[3] = {r_pin, g_pin, b_pin};
        int last_color = color - 1;
        if (last_color == -1) {
            last_color = 2;
        }

        analogWrite(rgb[color], counter);
        analogWrite(rgb[last_color], 255 - counter);
        counter++;

        if (counter == 256) {
            counter = 0;
            color++;

            if (color == 3) {
                color = 0;
            }
        }
    }

    void allFade() {
        int rgbw[4] = {r_pin, g_pin, b_pin, w_pin};
        int last_color = color - 1;
        if (last_color == -1) {
            last_color = 3;
        }

        analogWrite(rgbw[color], counter);
        analogWrite(rgbw[last_color], 255 - counter);
        counter++;

        if (counter == 256) {
            counter = 0;
            color++;

            if (color == 4) {
                color = 0;
            }
        }
        
    }

};