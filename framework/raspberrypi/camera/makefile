SHELL = /bin/sh
LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt
INCLUDES = -I$(PREFIX_GPIO) -I../include/
SOURCES = CCamera.cpp CFactoryCamera.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS =  $(SOURCES:%.cpp=%.h)
DEPENDENCIES =
ifeq (raspberry, $(OS_TYPE))
	ifneq (origin $(PREFIX_RASPICAM), undefined)
		RASPICAM_LIBS := -L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
		LIBS += $(RASPICAM_LIBS)
		INCLUDES += -I$(PREFIX_RASPICAM)/src
		SOURCES += COldCamera.cpp
		DEPENDENCIES += COldCamera.o
		HAS_OLD_CAMERA := 1
	endif
endif 

EXECUTABLES = libcamera.so
FLAGS = -std=gnu++11

all: $(EXECUTABLES)

CCamera.o: CCamera.cpp CCamera.h
	$(CXX) -fPIC -o CCamera.o -c CCamera.cpp $(INCLUDES) $(FLAGS)
	cp CCamera.h ../include

CFactoryCamera.o : CFactoryCamera.cpp CFactoryCamera.h CCamera.o $(DEPENDENCIES)
	$(CXX) -fPIC -c -o CFactoryCamera.o CFactoryCamera.cpp $(INCLUDES) $(FLAGS) -DHAS_OLD_CAMERA
	cp CFactoryCamera.h ../include

COldCamera.o : COldCamera.cpp COldCamera.o CCamera.o
	$(CXX) -fPIC -c -o COldCamera.o COldCamera.cpp $(INCLUDES) $(FLAGS)
	cp COldCamera.h ../include
	
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