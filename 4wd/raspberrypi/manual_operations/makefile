SHELL = /bin/sh

INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include  -I$(PREFIX_FRAMEWORK)/include -I../include
ifeq (raspberry, $(OS_TYPE))
	ifneq ($(origin $(PREFIX_RASPICAM)), undefined)
		INCLUDES += -I$(PREFIX_RASPICAM)/src
	endif
endif
SOURCES = CCommand.cpp CCommCommands.cpp CBLECommand.cpp CBLECommandHC05.cpp CMoveCommand.cpp  CSettingCommand.cpp CCommandStd.cpp CManualDroid.cpp CCommandEsp01.cpp CHttpCommand.cpp CSerialCommand.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS =  $(SOURCES:%.cpp=%.h)
EXECUTABLES =  libdroidcommands.so
FLAGS = -std=gnu++11 -ggdb

all: $(OBJECTS) $(EXECUTABLES)
	cd tests && $(MAKE)

CCommand.o:CCommand.cpp CCommand.h
	$(CXX) -fPIC -o CCommand.o -c CCommand.cpp $(INCLUDES) $(FLAGS)
	cp CCommand.h ../include
	
CCommCommands.o:CCommCommands.cpp CCommCommands.h
	$(CXX) -fPIC -o CCommCommands.o -c CCommCommands.cpp $(INCLUDES) $(FLAGS)
	cp CCommCommands.h ../include

CCommandStd.o:CCommandStd.cpp CCommandStd.h CCommCommands.o
	$(CXX) -fPIC -o CCommandStd.o -c CCommandStd.cpp $(INCLUDES) $(FLAGS)
	cp CCommandStd.h ../include

CBLECommandHC05.o:CBLECommandHC05.cpp CBLECommandHC05.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CBLECommandHC05.o -c CBLECommandHC05.cpp $(INCLUDES) $(FLAGS) -Wno-write-strings
	cp CBLECommandHC05.h ../include

CBLECommand.o:CBLECommand.cpp CBLECommand.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CBLECommand.o -c CBLECommand.cpp $(INCLUDES) $(FLAGS) -Wno-write-strings
	cp CBLECommand.h ../include

CCommandEsp01.o: CCommandEsp01.cpp CCommandEsp01.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CCommandEsp01.o -c CCommandEsp01.cpp $(INCLUDES) $(FLAGS) -Wno-write-strings
	cp CCommandEsp01.h ../include

CHttpCommand.o : CHttpCommand.cpp CHttpCommand.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CHttpCommand.o -c CHttpCommand.cpp $(INCLUDES) $(FLAGS) -Wno-write-strings
	cp CHttpCommand.h ../include
	
CSerialCommand.o : CSerialCommand.cpp CSerialCommand.h CCommand.o CCommCommands.o
	$(CXX) -fPIC -fpermissive -Wwrite-strings -o CSerialCommand.o -c CSerialCommand.cpp $(INCLUDES) $(FLAGS) -Wno-write-strings
	cp CSerialCommand.h ../include
	
CMoveCommand.o:CMoveCommand.cpp CMoveCommand.h CCommand.o
	$(CXX) -fPIC -o CMoveCommand.o -c CMoveCommand.cpp $(INCLUDES) $(FLAGS)
	cp CMoveCommand.h ../include

CSettingCommand.o:CSettingCommand.cpp CSettingCommand.h CCommand.o
	$(CXX) -fPIC -o CSettingCommand.o -c CSettingCommand.cpp $(INCLUDES) $(FLAGS)
	cp CSettingCommand.h ../include

CManualDroid.o:CManualDroid.cpp CManualDroid.h CCommCommands.o CSettingCommand.o CMoveCommand.o
	$(CXX) -fPIC -o CManualDroid.o -c CManualDroid.cpp $(INCLUDES) $(FLAGS)
	cp CManualDroid.h ../include
	
libdroidcommands.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libdroidcommands.so $(OBJECTS)
	cp libdroidcommands.so ../lib
		
clean:
	cd tests && $(MAKE) clean
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
	if [ -f ../lib/libdroidcommands.so ]; then rm ../lib/libdroidcommands.so; fi;
