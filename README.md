# Explorer

![Robot terminated 1](https://github.com/RICCIARDI-Adrien/Explorer/blob/master/Resources/Robot_Terminated_1.png)

A small wheeled robot based on a PIC18F26K22 microcontroller and a single ultrasonic distance sensor.  
The goal was to develop and test two basic artificial intelligence algorithms, both relying only on the ultrasonic sensor :
- The robot moves at will in a room and must avoid obstacles. It must never remain stuck.
- The robot is idle, waiting for a moving object to come close to it. Then, it must follow the moving object, staying at a safe distance. If the moving object comes too close from the robot, the robot must escape.

## Hardware
![Main board schematics](https://github.com/RICCIARDI-Adrien/Explorer/blob/master/Hardware/Schematics.png)

## Build tools
The schematics and PCB were drawn using Cadsoft Eagle 6.6.0.  
The microcontroller firmware is built with SourceBoost 7.30.  
The Command Line Interface program can be built under Linux using gcc or under Windows with Cygwin or MinGW.