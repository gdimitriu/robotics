CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar

LIBS = -lpthread -lpigpio -lrt -ldroiddrivers -lloggers -lsettings -ldroid -lsensors -ltof -lengines -lgrabber
LIB_DIRS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -L$(PREFIX_FRAMEWORK)/lib
INCLUDE_DIRS = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include  -I$(PREFIX_FRAMEWORK)/include -I../
OBJECTS = ../CCommand.o ../CMoveCommand.o ../CSettingCommand.o ../CCommCommands.o ../CCommandStd.o ../CBLECommandHC05.o ../CManualDroid.o
HEADERS = 
EXECUTABLES = testMenuStdout testMenuBle moveManualDroidStd moveManualDroidBleHC05
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)

testMenuStdout:testMenuStdout.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o testMenuStdout testMenuStdout.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

testMenuBle:testMenuBle.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o testMenuBle testMenuBle.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

moveManualDroidStd:moveManualDroidStd.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o moveManualDroidStd moveManualDroidStd.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

moveManualDroidBleHC05:moveManualDroidBleHC05.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o moveManualDroidBleHC05 moveManualDroidBleHC05.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)

clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(HEADERS);\
	do \
		if [ -f ../../include/$$object ]; then rm ../../include/$$object; fi; \
	done
