SHELL = /bin/sh

INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include
SOURCES = CLogger.cpp CLoggerStdout.cpp CLoggerBle.cpp CLoggerBleHC5.cpp CLoggerFile.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS =  $(SOURCES:%.cpp=%.h)
EXECUTABLES =  libloggers.so
FLAGS = -std=gnu++11 -DDEBUG_MODE

all: $(EXECUTABLES)

libloggers.so: $(OBJECTS)
	$(CXX) -fPIC -shared -o libloggers.so $(OBJECTS)
	cp libloggers.so ../lib

CLogger.o:CLogger.cpp CLogger.h
	$(CXX) -fPIC -o CLogger.o -c CLogger.cpp $(INCLUDES) $(FLAGS)
	cp CLogger.h ../include

CLoggerStdout.o:CLoggerStdout.cpp CLoggerStdout.h CLogger.h
	$(CXX) -fPIC -o CLoggerStdout.o -c CLoggerStdout.cpp $(INCLUDES) $(FLAGS)
	cp CLoggerStdout.h ../include

CLoggerBleHC5.o:CLoggerBleHC5.cpp CLoggerBleHC5.h CLoggerBle.o
	$(CXX) -fPIC -o CLoggerBleHC5.o -c CLoggerBleHC5.cpp $(INCLUDES) $(FLAGS) -fpermissive -Wwrite-strings
	cp CLoggerBleHC5.h ../include

CLoggerBle.o:CLoggerBle.cpp CLoggerBle.h CLogger.o
	$(CXX) -fPIC -o CLoggerBle.o -c CLoggerBle.cpp $(INCLUDES) $(FLAGS) -fpermissive
	cp CLoggerBle.h ../include

CLoggerFile.o:CLoggerFile.cpp CLoggerFile.h CLogger.h
	$(CXX) -fPIC -o CLoggerFile.o -c CLoggerFile.cpp $(INCLUDES) $(FLAGS)
	cp CLoggerFile.h ../include
		
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
	if [ -f ../lib/libloggers.so ]; then rm ../lib/libloggers.so; fi;
	
