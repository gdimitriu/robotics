CC = mipsel-openwrt-linux-g++
INCLUDEPATHS = -I$(PREFIX_DRIVERS)/include -I../include -I$(PREFIX_I2C_EXP_DRIVER)/include
LIBPATHS = -L$(PREFIX_I2C_EXP_DRIVER)/lib -L$(PREFIX_DRIVERS)/lib -L../lib
LIBS = -loniondebug -lonioni2c -lsx1509 -lengines -lthreadexecutors -lgpioisr -ladafruitpwmservo -lonionpwmexp
EXECUTABLES = calibrationMX1508SX1509 calibrationMX1508PCA9685 calibrationMX1508PWMExp

all: $(EXECUTABLES)

calibrationMX1508SX1509 : CalibrationMX1508SX1509.cpp
	$(CC) -fPIC -o calibrationMX1508SX1509 CalibrationMX1508SX1509.cpp \
	$(INCLUDEPATHS) $(LIBPATHS) $(LIBS)
	cp calibrationMX1508SX1509 ../bin

calibrationMX1508PCA9685: CalibrationMX1508PCA9685.cpp
	$(CC) -fPIC -o calibrationMX1508PCA9685 CalibrationMX1508PCA9685.cpp \
	$(INCLUDEPATHS) $(LIBPATHS) $(LIBS)
	cp calibrationMX1508PCA9685 ../bin
calibrationMX1508PWMExp: CalibrationMX1508PWMExp.cpp
	$(CC) -fPIC -o calibrationMX1508PWMExp CalibrationMX1508PWMExp.cpp \
	$(INCLUDEPATHS) $(LIBPATHS) $(LIBS)
	cp calibrationMX1508PWMExp ../bin
	
clean :
	for object in $(EXECUTABLES); \
	do \
		if [ -f $$object ]; then rm $$object; fi;\
		if [ -f ../bin/$$object ]; then rm ../bin/$$object; fi;\
	done
