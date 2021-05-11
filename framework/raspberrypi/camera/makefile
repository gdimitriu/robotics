CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
AR = arm-linux-gnueabihf-ar
RASPICAM_LIBS=-L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt $(RASPICAM_LIBS)
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_RASPICAM)/src -I../include/
OBJECTS =  CCamera.o
EXECUTABLES = libcamera.so
HEADERS =  CCamera.h
FLAGS = -std=gnu++11

all: $(EXECUTABLES)

CCamera.o: CCamera.cpp CCamera.h
	$(CXX) -fPIC -o CCamera.o -c CCamera.cpp $(INCLUDES) $(FLAGS)
	cp CCamera.h ../include
	
libcamera.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libcamera.so $(OBJECTS)
	cp libcamera.so ../lib
	
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
	if [ -f ../lib/libcamera.so ]; then rm ../lib/libcamera.so; fi;