CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar

RASPICAM_LIBS=-L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
LIBS = -lpthread -lpigpio -lrt -lengines -ldroiddrivers -ldroid -lgrabber -lcamera $(RASPICAM_LIBS)

LIB_DIRS = -L$(PREFIX_GPIO) -L$(PREFIX_DRIVERS)/lib -I$(PREFIX_FRAMEWORK)/lib
INCLUDE_DIRS = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include  -I$(PREFIX_FRAMEWORK)/include -I../include -I$(PREFIX_RASPICAM)/src
OBJECTS = CCommand.o CCommCommands.o CBLECommand.o CBLECommandHC05.o CMoveCommand.o  CSettingCommand.o CCommandStd.o CManualDroid.o CCommandEsp01.o
HEADERS = CCommand.h CCommCommands.h CCommandStd.h CBLECommand.h CBLECommandHC05.h CMoveCommand.h CSettingCommand.h CManualDroid.h CCommandEsp01.h
EXECUTABLES =  
FLAGS = -std=gnu++11 -ggdb

all: $(OBJECTS)
	cd tests && make

CCommand.o:CCommand.cpp CCommand.h
	$(CXX) -fPIC -o CCommand.o -c CCommand.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS)
	cp CCommand.h ../include
	
CCommCommands.o:CCommCommands.cpp CCommCommands.h
	$(CXX) -fPIC -o CCommCommands.o -c CCommCommands.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS)
	cp CCommCommands.h ../include

CCommandStd.o:CCommandStd.cpp CCommandStd.h CCommCommands.o
	$(CXX) -fPIC -o CCommandStd.o -c CCommandStd.cpp $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS) $(FLAGS)
	cp CCommandStd.h ../include

CBLECommandHC05.o:CBLECommandHC05.cpp CBLECommandHC05.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CBLECommandHC05.o -c CBLECommandHC05.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS) -Wno-write-strings
	cp CBLECommandHC05.h ../include

CBLECommand.o:CBLECommand.cpp CBLECommand.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CBLECommand.o -c CBLECommand.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS) -Wno-write-strings
	cp CBLECommand.h ../include

CCommandEsp01.o: CCommandEsp01.cpp CCommandEsp01.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CCommandEsp01.o -c CCommandEsp01.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS) -Wno-write-strings
	cp CCommandEsp01.h ../include
	
CMoveCommand.o:CMoveCommand.cpp CMoveCommand.h CCommand.o
	$(CXX) -fPIC -o CMoveCommand.o -c CMoveCommand.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS)
	cp CMoveCommand.h ../include

CSettingCommand.o:CSettingCommand.cpp CSettingCommand.h CCommand.o
	$(CXX) -fPIC -o CSettingCommand.o -c CSettingCommand.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS)
	cp CSettingCommand.h ../include

CManualDroid.o:CManualDroid.cpp CManualDroid.h CCommCommands.o CSettingCommand.o CMoveCommand.o
	$(CXX) -fPIC -o CManualDroid.o -c CManualDroid.cpp $(LIBS) $(LIB_DIRS) $(INCLUDE_DIRS) $(FLAGS)
	cp CManualDroid.h ../include
	
clean:
	cd tests && make clean
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
