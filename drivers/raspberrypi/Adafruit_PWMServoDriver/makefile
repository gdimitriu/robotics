SHELL = /bin/sh

INCLUDEPATHS = -I$(PREFIX_GPIO) -I../include

all: Adafruit_PWMServoDriver.o

Adafruit_PWMServoDriver.o : Adafruit_PWMServoDriver.cpp Adafruit_PWMServoDriver.h
	$(CXX) -fPIC -c $(INCLUDEPATHS) Adafruit_PWMServoDriver.cpp -o ../obj/Adafruit_PWMServoDriver.o
	cp Adafruit_PWMServoDriver.h ../include	
	
clean :
	if [ -f ../obj/Adafruit_PWMServoDriver.o ]; then rm ../obj/Adafruit_PWMServoDriver.o; fi;
	if [ -f ../include/Adafruit_PWMServoDriver.h ]; then rm ../include/Adafruit_PWMServoDriver.h; fi;