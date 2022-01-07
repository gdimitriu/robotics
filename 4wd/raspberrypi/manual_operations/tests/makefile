CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar

RASPICAM_LIBS=-L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
LIBS = -lpthread -lpigpio -lrt -ldroiddrivers -lloggers -lsettings -ldroid -lsensors -ltof -lengines -lgrabber -lcamera -ldroidcommands $(RASPICAM_LIBS)
LIB_DIRS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -L$(PREFIX_FRAMEWORK)/lib -L../../lib
INCLUDE_DIRS = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include  -I$(PREFIX_FRAMEWORK)/include -I../ -I$(PREFIX_RASPICAM)/src
OBJECTS = 
HEADERS = 
EXECUTABLES = testMenuStdout testMenuBle moveManualDroidStd moveManualDroidBleHC05 moveManualDroidBle moveManualDroidEsp01
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)

testMenuStdout:testMenuStdout.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o testMenuStdout testMenuStdout.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

testMenuBle:testMenuBle.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o testMenuBle testMenuBle.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

moveManualDroidStd:moveManualDroidStd.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ./CCommandStd.h
	$(CXX) -fPIC -o moveManualDroidStd moveManualDroidStd.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

moveManualDroidBleHC05:moveManualDroidBleHC05.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ./CCommandHC05.h
	$(CXX) -fPIC -o moveManualDroidBleHC05 moveManualDroidBleHC05.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

moveManualDroidBle:moveManualDroidBle.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ./CCommandBle.h
	$(CXX) -fPIC -o moveManualDroidBle moveManualDroidBle.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS) -Wno-write-strings

moveManualDroidEsp01: moveManualDroidEsp01.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ../CCommandEsp01.h
	$(CXX) -fPIC -o moveManualDroidEsp01 moveManualDroidEsp01.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS) -Wno-write-strings
	
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(HEADERS);\
	do \
		if [ -f ../../include/$$object ]; then rm ../../include/$$object; fi; \
	done
