CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar

RASPICAM_LIBS=-L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
LIBS = -lpthread -lpigpio -lrt -ldroiddrivers -lloggers -ldroid -lengines -lsettings -lsensors -ltof -lgrabber -lcamera $(RASPICAM_LIBS)
LIB_DIRS =  -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -L../../lib
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../../include
OBJECTS = 
HEADERS = 
EXECUTABLES =  testLoadDroid firstMove firstRotate firstMoveRotate testLoadDroidInit
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)
	
testLoadDroid:testLoadDroid.cpp
	$(CXX) -fPIC -o testLoadDroid testLoadDroid.cpp $(INCLUDES) $(FLAGS) $(LIBS) $(LIB_DIRS)

testLoadDroidInit:testLoadDroidInit.cpp
	$(CXX) -fPIC -o testLoadDroidInit testLoadDroidInit.cpp $(INCLUDES) $(FLAGS) $(LIBS) $(LIB_DIRS)

firstMove:firstMove.cpp
	$(CXX) -fPIC -o firstMove firstMove.cpp $(INCLUDES) $(FLAGS) $(LIBS) $(LIB_DIRS)

firstRotate:firstRotate.cpp
	$(CXX) -fPIC -o firstRotate firstRotate.cpp $(INCLUDES) $(FLAGS) $(LIBS) $(LIB_DIRS)

firstMoveRotate:firstMoveRotate.cpp
	$(CXX) -fPIC -o firstMoveRotate firstMoveRotate.cpp $(INCLUDES) $(FLAGS) $(LIBS) $(LIB_DIRS)

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
	
