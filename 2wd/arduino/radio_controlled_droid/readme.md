# Droid with 2 weels or two propellers

## Radio commanded Track droid

![Track droid](docs/track_droid.jpg) 

## Components

- Arduino uno
- receiver 433 Mhz
- micro engine driver
- pololu track
- two without encoder engines
- 9V accumulator
- wires


## OBS

TX is made using a pair of RF 433Mhz emiter and arduino.

Control is make using joystick.

This is done using RF 433MHz pair or emiter and receiver like 

https://www.optimusdigital.ro/ro/ism-433-mhz/252-pereche-emitator-si-receptor-rf-433-mhz.html
 
The commands are similar with one use used by BLE or serial command.

The lateral brackets are intended to be used by U encoders.

Front bracket is used to hold pixy2 camera or two HC-SR04P.

This chasy was used for other old projects:

- line_pixy2_demo_L298N (L298N standalone)
- line_pixy2_demo_shield (using arduino shield L298N)
- pixy2_L298N_barcode_LR