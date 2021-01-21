#makefile for calibrations of engines framework
CC = mipsel-openwrt-linux-g++

INCLUDEPATHS = -I../../include -I$(PREFIX_DRIVERS)/include -I$(PREFIX_I2C_EXP_DRIVER)/include -I../

LIBPATHS = -L$(PREFIX_DRIVERS)/lib -L$(PREFIX_I2C_EXP_DRIVER)/lib -L../../lib -L../

LIBS = -lgpioisr -lregisteroperations -lthreadexecutors -lonioni2c -loniondebug -lengines -lsx1509 -ladafruitpwmservo -lonionpwmexp

all : EncodersCalibrations
	cp EncodersCalibrations ../../bin
	
EncodersCalibrations: EncodersCalibrations.cpp
	$(CC) -fPIC -o EncodersCalibrations EncodersCalibrations.cpp $(INCLUDEPATHS) \
	$(LIBPATHS) $(LIBS)
	
clean:
	if [ -f EncodersCalibrations ]; then rm EncodersCalibrations; fi;
	if [ -f ../../bin/EncodersCalibrations ]; then rm ../../bin/EncodersCalibrations; fi;