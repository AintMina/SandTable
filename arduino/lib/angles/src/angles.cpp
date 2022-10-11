/*

Author: AintMina
Date: 25.3.2021
Version 0.5
 
Library to get angles for 2-joint robotic arm.

*/

#include <math.h>
#include "angles.hpp"


// Get angles for arm 2 from polar coordinates
float polarGetTheta2(float theta, float r) {

    // Math (had to break it up to multiple variables)
    float s1 = pow(r, 2) - 0.5;
    float q2 = acos(s1 / 0.5);

    return q2;
}

// Get angles for arm 1 from polar coordinates
float polarGetTheta1(float theta2, float theta, float r) {

    // Checking if denominator is zero
    float s2 = 0.5 + 0.5 * cos(theta2);
    if (s2 == 0.0) {
        s2 = 0.000001;
    }

    // Math
    float s1 = 0.5 * sin(theta2);
    float alpha = atan(s1 / s2);
    float q1 = theta - alpha;
    

    return q1;
}

// Calculating change of angles
float deltaAngles(float theta1, float theta_old) {

    float delta_theta = theta1 - theta_old;

    while (delta_theta > 5) {
        delta_theta -= (2*M_PI);
    }
    while (delta_theta < -5) {
        delta_theta += (2*M_PI);
    }

    return delta_theta;
}

// Calculating steps from delta angles
int steps(float theta, int microstepping) {

    if (theta == 0.00000) {
        return 0;
    }

    unsigned long steps_per_revolution = 600 * microstepping;
    float s1 = (steps_per_revolution / (2 * M_PI));
    int step = round(s1 * theta);

    return step;
}

float thetaFromSteps(int step, int microstepping) {

    if (step == 0) {
        return 0.0;
    }

    unsigned int steps_per_revolution = 600 * microstepping;
    float theta = step / (steps_per_revolution / (2 * M_PI));

    return theta;
}

float thetaFromArms(float q1, float q2) {

    float x = 0.5 * cos(q1) + 0.5 * cos(q1 + q2);
    float y = 0.5 * sin(q1) + 0.5 * sin(q1 + q2);

    if(x == 0 && y == 0) {
        return 0.0f;
    }

    float theta = atan2(y, x);

    return theta;
}

float rFromArms(float q1, float q2) {

    float x = 0.5 * cos(q1) + 0.5 * cos(q1 + q2);
    float y = 0.5 * sin(q1) + 0.5 * sin(q1 + q2);

    if(x == 0 && y == 0) {
        return 0.0f;
    }

    float r2 = pow(x, 2) + pow(y, 2);

    return sqrt(r2);
}
