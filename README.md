# ROBOTICS
## droids
droids using Arduino, Onion Omega2+, esp32-cam and RaspberryPi Zero W

### 2 wheel drive droid using Onion Omega 2+
[2wd_droid](2wd/onionOmega2/README.md)
![2wd_onion](2wd/onionOmega2/docs/2wd_onion.jpg)

### 4 wheel drive droid using Arduino
[4wd_droid](4wd/arduino/readme.md)

![4wd_doid_chasy](4wd/arduino/docs/4wd_pixy_droid.jpg)

### Raspberry Pi Zero W droid using linux and RT

This has been moved to it's own repository [RaspberryPi Zero W droid](https://github.com/gdimitriu/raspberrypiZeroDroid) because it is on halt due to lack of processors.


### Zumo droid using Arduino [zumo_droid](zumo_droid/README.md)

![zumo_droid](zumo_droid/arduino/docs/zumo_droid.jpg)

### track_droid (DYI chasy)

#### arduino 

This is a robot using Pix2Cam using arduino.

This has been moved it's own repository [pixy2Droid](https://github.com/gdimitriu/pixy2Droid)

#### ESP32-CAM

This is a robot using ESP32-CAM with normal move and streaming of images.

This could be controlled using [robot_control](https://github.com/gdimitriu/tcp_tools/tree/main/robot_controll).

This has been moved it's own repository [swarm_of_robots](https://github.com/gdimitriu/swarm_robots/tree/main/surveillance/esp32-cam)

### robot_arm

This is poc with utils for a miniarm using arduino.

This has been moved with android application it's own repository [miniarm](https://github.com/gdimitriu/miniarm)

## tracking_turret

This is a tracking turret using arduino and two HC SR-04 ultrasonic sensors.

It should create a map and then finding a new object it should point with a laser.

In this moment is now working as predicted.

## quadcopter

This is a quadcopter using arduino.

It never left the gound so this project is stopped.

## drivers

This contains some tests using RX/TX, AP, WIFI.

This was inteded to create drivers but finally it contains tests.

## poc_tests

This contains proof of concepts and tests using different microcontrolers.

### [arduino](poc_tests/arduino/README.md)

This contains different poc and test using arduino (like sensors, motors).

### ESP32

- SimpleEchoWIFIServer is a echo server using wifi

- esp32_cam_commands is a poc for receiving commands for a droid using wifi

- esp32_cam_commands is the same as esp32_cam_commands but with streaming using the code from the example from expresif.

### [raspberry pico](poc_tests/pico/README.md)


## utils

This contains different program used for callibration or validation of a microcontroller robot platform.

### [arduino](utils/arduino/README.md)

### [raspberryPi](utils/raspberryPi/README.md)

### tcp_terminal

This is the first version the full version has it's own repository at [tcp_terminal](https://github.com/gdimitriu/tcp_tools/tree/main/tcp_terminal)
