SHELL = /bin/sh

RASPICAM_LIBS=-L$(PREFIX_RASPICAM)/build/src -L$(PREFIX_VC)/lib -lraspicam -lmmal -lmmal_core -lvcos -lmmal_util -lmmal_vc_client -lmmal_components -lvchiq_arm -lvcsm -lcontainers
LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt $(RASPICAM_LIBS)
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_RASPICAM)/src
OBJECTS =  
EXECUTABLES = cameraBle 

all: $(EXECUTABLES)

cameraBle:cameraBle.cpp
	$(CXX) -fPIC -o cameraBle cameraBle.cpp $(INCLUDES) $(LIBS)
		
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
