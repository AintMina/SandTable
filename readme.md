This is an engineering project to build a Sisyphus table -like kinetic art table.


The first stage of the project is to build a 2 link planar robotic arm. The arm will have a working area of 360 degrees with a radius of around 280 mm.
Parts have been designed with Fusion 360 by me where the inspiration and help came from [grammesm](https://alwaystinkering.com/2020/01/14/diy-kinetic-sand-art-table/). 
For the axis, I noticed that plastic was not rigid enough. So I used an 8 mm aluminium rod to fit through skateboard bearings. The gear ratio I used is 20/60 where the belt lengths are 158 mm for the first axis and 400 mm for the 140.3 mm arm.


To control the arm, the first tests were made using raspberry pi and python. The stepper motor hat by Waveshare utilized dual stepper motor plugs and used DRV8825 stepper motor drivers to control the motors. After weeks of writing python scripts for raspberry, I realized that the DRV8825 was way too loud for my liking, so I decided to search for alternatives. The stepper motor hat had integrated drivers, which meant that I had to look for a new solution entirely and found a CNC shield for Arduino.

Going from raspberry pi to Arduino meant that I had to go from Python to C++, which made it hard because I had not used it before. But after weeks of struggling, I managed to build a prototype code to test everything.

So far, I'm barely managing, but I'm learning a lot from this which was the whole point of the project.