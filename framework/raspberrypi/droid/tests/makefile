SHELL = /bin/sh

LIBS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -L../../lib -lpthread -lpigpio -lrt -ldroiddrivers -lloggers -ldroid -lengines -lsettings -lsensors -ltof -lgrabber -ldroidcamera
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../../include
ifeq (raspberry,$(OS_TYPE))
	ifneq ($(origin $(PREFIX_RASPICAM)), undefined)
		RASPICAM_LIBS = -L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
		LIBS += $(RASPICAM_LIBS)
		INCLUDES += -I$(PREFIX_RASPICAM)/src
	endif
endif
OBJECTS = 
HEADERS = 
EXECUTABLES =  testLoadDroid firstMove firstRotate firstMoveRotate testLoadDroidInit
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)
	
testLoadDroid:testLoadDroid.cpp
	$(CXX) -fPIC -o testLoadDroid testLoadDroid.cpp $(INCLUDES) $(FLAGS) $(LIBS)

testLoadDroidInit:testLoadDroidInit.cpp
	$(CXX) -fPIC -o testLoadDroidInit testLoadDroidInit.cpp $(INCLUDES) $(FLAGS) $(LIBS)

firstMove:firstMove.cpp
	$(CXX) -fPIC -o firstMove firstMove.cpp $(INCLUDES) $(FLAGS) $(LIBS)

firstRotate:firstRotate.cpp
	$(CXX) -fPIC -o firstRotate firstRotate.cpp $(INCLUDES) $(FLAGS) $(LIBS)

firstMoveRotate:firstMoveRotate.cpp
	$(CXX) -fPIC -o firstMoveRotate firstMoveRotate.cpp $(INCLUDES) $(FLAGS) $(LIBS)

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
	
