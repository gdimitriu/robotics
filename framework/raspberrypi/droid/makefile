CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar

RASPICAM_LIBS=-L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include -I$(PREFIX_RASPICAM)/src
OBJECTS = CDroid.o
HEADERS = CDroid.h
EXECUTABLES =  libdroid.so
FLAGS = -std=gnu++11

all: $(EXECUTABLES)
	cd tests && make
	
libdroid.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libdroid.so $(OBJECTS)
	cp libdroid.so ../lib

CDroid.o:CDroid.cpp CDroid.h
	$(CXX) -fPIC -o CDroid.o -c CDroid.cpp $(INCLUDES) $(FLAGS)
	cp CDroid.h ../include
			
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
	if [ -f ../lib/libdroid.so ]; then rm ../lib/libdroid.so; fi;
	cd tests && make clean
