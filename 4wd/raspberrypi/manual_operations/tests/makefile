SHELL = /bin/sh

LIBS = -lpthread -lpigpio -lrt -ldroiddrivers -lloggers -lsettings -ldroid -lsensors -ltof -lengines -lgrabber -ldroidcamera -ldroidcommands
LIB_DIRS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -L$(PREFIX_FRAMEWORK)/lib -L../../lib
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include  -I$(PREFIX_FRAMEWORK)/include -I../ 
ifeq (raspberry, $(OS_TYPE))
	ifneq ($(origin $(PREFIX_RASPICAM)), undefined)
		INCLUDES += -I$(PREFIX_RASPICAM)/src
		RASPICAM_LIBS = -L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
		LIBS += $(RASPICAM_LIBS)
	endif
endif	

EXECUTABLES = testMenuStdout testMenuBle moveManualDroidStd moveManualDroidBleHC05 moveManualDroidBle moveManualDroidEsp01 moveManualDroidWifiBleStd

FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)

testMenuStdout:testMenuStdout.cpp ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o testMenuStdout testMenuStdout.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS)

testMenuBle:testMenuBle.cpp ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h
	$(CXX) -fPIC -o testMenuBle testMenuBle.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS)

moveManualDroidStd:moveManualDroidStd.cpp ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ../CCommandStd.h
	$(CXX) -fPIC -o moveManualDroidStd moveManualDroidStd.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS)

moveManualDroidBleHC05:moveManualDroidBleHC05.cpp $(OBJECTS) ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ../CBLECommandHC05.h
	$(CXX) -fPIC -o moveManualDroidBleHC05 moveManualDroidBleHC05.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS)

moveManualDroidBle:moveManualDroidBle.cpp ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ../CBLECommand.h
	$(CXX) -fPIC -o moveManualDroidBle moveManualDroidBle.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS) -Wno-write-strings

moveManualDroidEsp01: moveManualDroidEsp01.cpp ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ../CCommandEsp01.h
	$(CXX) -fPIC -o moveManualDroidEsp01 moveManualDroidEsp01.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS) -Wno-write-strings

moveManualDroidWifiBleStd: moveManualDroidWifiBleStd.cpp ../CCommand.h ../CMoveCommand.h ../CSettingCommand.h ../CCommandEsp01.h ../CBLECommand.h ../CCommandStd.h
	$(CXX) -fPIC -o moveManualDroidWifiBleStd moveManualDroidWifiBleStd.cpp $(OBJECTS) $(LIB_DIRS) $(LIBS) $(INCLUDES) $(FLAGS) -Wno-write-strings
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(HEADERS);\
	do \
		if [ -f ../../include/$$object ]; then rm ../../include/$$object; fi; \
	done
