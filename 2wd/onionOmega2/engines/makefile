#makefile for engines
CC = mipsel-openwrt-linux-g++

INCLUDEPATHS = -I./ -I$(PREFIX_DRIVERS)/include -I$(PREFIX_I2C_EXP_DRIVER)/include -I../include

#this is need by GPIO and PWM operations directly on omega2+ pins
INCLUDE_GPIO = $(PREFIX_FAST_GPIO)/src/fastgpioomega2.cpp $(PREFIX_FAST_GPIO)/src/fastpwm.cpp $(PREFIX_FAST_GPIO)/src/module.cpp -I$(PREFIX_FAST_GPIO)/include

OBJECTS = Engines.o EncoderObserver.o EnginesWithEncoder.o EnginesWithEncoderMX1509SX1509.o EnginesWithEncoderMX1509PCA9685.o EnginesWithEncoderMX1509.o EnginesWithEncoderMX1509PWMExp.o
HEADERS = Engines.h EnginesWithEncoder.h EncoderObserver.h EnginesWithEncoderMX1509SX1509.h EnginesWithEncoderMX1509PCA9685.h EnginesWithEncoderMX1509.h EnginesWithEncoderMX1509PWMExp.h

all: engines
	cd calibrations && make

engines: $(OBJECTS)
	$(CC) -shared -fPIC -Wl,--allow-multiple-definition -o libengines.so $(OBJECTS)
	cp libengines.so ../lib
	cp $(HEADERS) ../include

Engines.o : Engines.cpp $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) Engines.cpp

EncoderObserver.o : EncoderObserver.cpp $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) EncoderObserver.cpp

EnginesWithEncoder.o: EnginesWithEncoder.cpp $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) EnginesWithEncoder.cpp

EnginesWithEncoderMX1509PCA9685.o: EnginesWithEncoderMX1509PCA9685.cpp $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) EnginesWithEncoderMX1509PCA9685.cpp

EnginesWithEncoderMX1509.o: EnginesWithEncoderMX1509.cpp $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) EnginesWithEncoderMX1509.cpp

EnginesWithEncoderMX1509SX1509.o: EnginesWithEncoderMX1509SX1509.cpp  $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) EnginesWithEncoderMX1509SX1509.cpp

EnginesWithEncoderMX1509PWMExp.o: EnginesWithEncoderMX1509PWMExp.cpp $(HEADERS)
	$(CC) -fPIC -c $(INCLUDEPATHS) $(INCLUDE_GPIO) EnginesWithEncoderMX1509PWMExp.cpp
	
clean:
	if [ -f libengines.so ]; then rm libengines.so; fi;
	if [ -f fastgpioomega2.o ]; then rm fastgpioomega2.o; fi;
	if [ -f fastpwm.o ]; then rm fastpwm.o; fi;
	if [ -f module.o ]; then rm module.o; fi;
	for object in $(OBJECTS); \
	do \
		if [ -f $$object ]; then rm $$object; fi;\
	done
	if [ -f ../lib/libengines.so ]; then rm ../lib/libengines.so; fi;
	for object in $(HEADERS);\
	do \
		if [ -f ../include/$$object ]; then rm ../include/$$object; fi;\
	done
	cd calibrations && make clean