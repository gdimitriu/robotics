CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar

LIBS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -lpthread -lpigpio -lrt -ladafruitpwmservo -lsettings -llogger -L../lib
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include
OBJECTS = CEnginePCA9685EncMX1509.o CEngineWithEncoder.o CFactoryEnginePCA9685EncMX1509.o  CFactoryEngine.o \
		CMasterControlEngines.o CResetableBarrier.o
HEADERS = CEnginePCA9685EncMX1509.h CEngineWithEncoder.h CFactoryEnginePCA9685EncMX1509.h CFactoryEngine.h \
		CMasterControlEngines.h CResetableBarrier.h
EXECUTABLES =  libengines.so
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)
	cd tests && make
	
libengines.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libengines.so $(OBJECTS)
	cp libengines.so ../lib

CResetableBarrier.o: CResetableBarrier.cpp CResetableBarrier.h
	$(CXX) -fPIC -o CResetableBarrier.o -c CResetableBarrier.cpp $(INCLUDES) $(FLAGS)
	cp CResetableBarrier.h ../include

CMasterControlEngines.o:CMasterControlEngines.cpp CMasterControlEngines.h CFactoryEnginePCA9685EncMX1509.o CResetableBarrier.o
	$(CXX) -fPIC -o CMasterControlEngines.o -c CMasterControlEngines.cpp $(INCLUDES) $(FLAGS)
	cp CMasterControlEngines.h ../include
	
CEnginePCA9685EncMX1509.o:CEnginePCA9685EncMX1509.cpp CEnginePCA9685EncMX1509.h CEngineWithEncoder.o CResetableBarrier.o
	$(CXX) -fPIC -o CEnginePCA9685EncMX1509.o -c CEnginePCA9685EncMX1509.cpp $(INCLUDES) $(FLAGS)
	cp CEnginePCA9685EncMX1509.h ../include

CEngineWithEncoder.o:CEngineWithEncoder.cpp CEngineWithEncoder.h CResetableBarrier.o
	$(CXX) -fPIC -o CEngineWithEncoder.o -c CEngineWithEncoder.cpp $(INCLUDES) $(FLAGS)
	cp CEngineWithEncoder.h ../include
	
CFactoryEngine.o:CFactoryEngine.cpp CFactoryEngine.h  
	$(CXX) -fPIC -o CFactoryEngine.o -c CFactoryEngine.cpp $(INCLUDES) $(FLAGS)
	cp CFactoryEngine.h ../include
	
CFactoryEnginePCA9685EncMX1509.o:CFactoryEnginePCA9685EncMX1509.cpp CFactoryEnginePCA9685EncMX1509.h  \
	CEnginePCA9685EncMX1509.o CFactoryEngine.o
	$(CXX) -fPIC -o CFactoryEnginePCA9685EncMX1509.o -c CFactoryEnginePCA9685EncMX1509.cpp $(INCLUDES) $(FLAGS)
	cp CFactoryEnginePCA9685EncMX1509.h ../include
				
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
	if [ -f ../lib/libengines.so ]; then rm ../lib/libengines.so; fi;
	cd tests && make clean
	
