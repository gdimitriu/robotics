SHELL = /bin/sh

LIBS =  -lpthread -lpigpio -lrt -lengines -lsettings -lloggers -ldroiddrivers -lsensors -ltof
LIB_DIRS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -L../../lib
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../../include
OBJECTS = 
HEADERS = 
EXECUTABLES =  loadTest createMasterTest createMasterTestBle
FLAGS = -std=gnu++11

all: $(EXECUTABLES)

loadTest:loadTest.cpp
	$(CXX) -fPIC -o loadTest loadTest.cpp $(LIBS) $(LIB_DIRS) $(INCLUDES) $(FLAGS)

createMasterTest:createMasterTest.cpp
	$(CXX) -fPIC -o createMasterTest createMasterTest.cpp $(LIBS) $(LIB_DIRS) $(INCLUDES) $(FLAGS)

createMasterTestBle:createMasterTestBle.cpp
	$(CXX) -fPIC -o createMasterTestBle createMasterTestBle.cpp $(LIBS) $(LIB_DIRS) $(INCLUDES) $(FLAGS)
	 			
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
