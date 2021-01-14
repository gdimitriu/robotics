# DROID framework for RaspberryPI

This project is a droid controled from command line on ble or std input/output, with front sensor to guard 
against objects in front and when it take left/right turn.

## Build:

Modify the settings.sh to match your build directory of :

- PREFIX_GPIO to libs of https://github.com/joan2937/pigpio
- PREFIX_DRIVERS to drivers from https://github.com/gdimitriu/raspberrypizero.git

## UML Diagram of the Droid engines and sensors without factories.

![UML Diagram](./docs/control_diagram.jpeg)