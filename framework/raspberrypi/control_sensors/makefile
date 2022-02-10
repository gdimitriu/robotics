SHELL = /bin/sh

INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include
SOURCES = CMasterControlSensors.cpp CGenericSensor.cpp CSweepPCA9685.cpp CToFSweepPCA9685.cpp CFactoryToFSweepPCA9685.cpp \
	CFactorySensor.cpp CFactorySensors.cpp CFixDigitalSensor.cpp CFactoryFixDigitalSensor.cpp CFactoryHCSR04SweepPCA9685.cpp \
	CHCSR04SweepPCA9685.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS =  $(SOURCES:%.cpp=%.h)
EXECUTABLES =  libsensors.so
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)
	
libsensors.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libsensors.so $(OBJECTS)
	cp libsensors.so ../lib
	
CMasterControlSensors.o:CMasterControlSensors.cpp CMasterControlSensors.h CGenericSensor.o CFactorySensors.o
	$(CXX) -fPIC -o CMasterControlSensors.o -c CMasterControlSensors.cpp $(INCLUDES) $(FLAGS)
	cp CMasterControlSensors.h ../include

CGenericSensor.o:CGenericSensor.cpp CGenericSensor.h
	$(CXX) -fPIC -o CGenericSensor.o -c CGenericSensor.cpp $(INCLUDES) $(FLAGS)
	cp CGenericSensor.h ../include

CSweepPCA9685.o:CSweepPCA9685.cpp CSweepPCA9685.h CGenericSensor.o
	$(CXX) -fPIC -o CSweepPCA9685.o -c CSweepPCA9685.cpp $(INCLUDES) $(FLAGS)
	cp CSweepPCA9685.h ../include
	
CToFSweepPCA9685.o:CToFSweepPCA9685.cpp CToFSweepPCA9685.h CSweepPCA9685.o
	$(CXX) -fPIC -o CToFSweepPCA9685.o -c CToFSweepPCA9685.cpp $(INCLUDES) $(FLAGS)
	cp CToFSweepPCA9685.h ../include
	
CHCSR04SweepPCA9685.o:CHCSR04SweepPCA9685.cpp CHCSR04SweepPCA9685.h CSweepPCA9685.o
	$(CXX) -fPIC -o CHCSR04SweepPCA9685.o -c CHCSR04SweepPCA9685.cpp $(INCLUDES) $(FLAGS)
	cp CHCSR04SweepPCA9685.h ../include
	
CFactoryToFSweepPCA9685.o:CFactoryToFSweepPCA9685.cpp CFactoryToFSweepPCA9685.h CToFSweepPCA9685.o CFactorySensor.o
	$(CXX) -fPIC -o CFactoryToFSweepPCA9685.o -c CFactoryToFSweepPCA9685.cpp $(INCLUDES) $(FLAGS)
	cp CFactoryToFSweepPCA9685.h ../include
	
CFactoryHCSR04SweepPCA9685.o:CFactoryHCSR04SweepPCA9685.cpp CFactoryHCSR04SweepPCA9685.h CHCSR04SweepPCA9685.o CFactorySensor.o
	$(CXX) -fPIC -o CFactoryHCSR04SweepPCA9685.o -c CFactoryHCSR04SweepPCA9685.cpp $(INCLUDES) $(FLAGS)
	cp CFactoryHCSR04SweepPCA9685.h ../include
	
CFactorySensor.o:CFactorySensor.cpp CFactorySensor.h
	$(CXX) -fPIC -o CFactorySensor.o -c CFactorySensor.cpp $(INCLUDES) $(FLAGS)
	cp CFactorySensor.h ../include

CFactorySensors.o:CFactorySensors.cpp CFactorySensors.h CFactorySensor.o CFactoryToFSweepPCA9685.o CFactoryFixDigitalSensor.o CFactoryHCSR04SweepPCA9685.o
	$(CXX) -fPIC -o CFactorySensors.o -c CFactorySensors.cpp $(INCLUDES) $(FLAGS)
	cp CFactorySensors.h ../include
	
CFixDigitalSensor.o:CFixDigitalSensor.cpp CFixDigitalSensor.h CGenericSensor.o
	$(CXX) -fPIC -o CFixDigitalSensor.o -c CFixDigitalSensor.cpp $(INCLUDES) $(FLAGS)
	cp CFixDigitalSensor.h ../include

CFactoryFixDigitalSensor.o:CFactoryFixDigitalSensor.cpp CFactoryFixDigitalSensor.h CFixDigitalSensor.o
	$(CXX) -fPIC -o CFactoryFixDigitalSensor.o -c CFactoryFixDigitalSensor.cpp $(INCLUDES) $(FLAGS)
	cp CFactoryFixDigitalSensor.h ../include
	
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(HEADERS);\
	do \
		if [ -f ../include/$$object ]; then rm ../include/$$object; fi; \
	done
	if [ -f ../lib/libsensors.so ]; then rm ../lib/libsensors.so; fi;
	
