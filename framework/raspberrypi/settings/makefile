SHELL = /bin/sh

INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include
SOURCES = CSettingLoading.cpp CFactoryLogger.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS =  $(SOURCES:%.cpp=%.h)
EXECUTABLES =  libsettings.so
FLAGS = -std=gnu++11

all: $(EXECUTABLES)
	
libsettings.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libsettings.so $(OBJECTS)
	cp libsettings.so ../lib

CSettingLoading.o:CSettingLoading.cpp CSettingLoading.h
	$(CXX) -fPIC -o CSettingLoading.o -c CSettingLoading.cpp $(INCLUDES) $(FLAGS)
	cp CSettingLoading.h ../include

CFactoryLogger.o:CFactoryLogger.cpp CFactoryLogger.h CSettingLoading.o
	$(CXX) -fPIC -o CFactoryLogger.o -c CFactoryLogger.cpp $(INCLUDES) $(FLAGS)
	cp CFactoryLogger.h ../include
			
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
	if [ -f ../lib/libsettings.so ]; then rm ../lib/libsettings.so; fi;
	
