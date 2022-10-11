/*

Author: AintMina
Date: 11.10.2022
Version 0.1
 
Library to get angles for 2-joint robotic arm.

*/

#ifndef ANGLES_H
#define ANGLES_H

float polarGetTheta2(float theta, float r);
float polarGetTheta1(float theta2, float theta, float r);
float deltaAngles(float theta1, float theta_old);
int steps(float theta, int microstepping);
float thetaFromSteps(int step, int microstepping);
float thetaFromArms(float q1, float q2);
float rFromArms(float q1, float q2);

#endif