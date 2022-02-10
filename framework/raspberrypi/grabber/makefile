SHELL = /bin/sh

INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include
SOURCES = CGrabber.cpp CGrabberController.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS =  $(SOURCES:%.cpp=%.h)
EXECUTABLES =  libgrabber.so
FLAGS = -std=gnu++11

all: $(EXECUTABLES)
	
libgrabber.so: $(OBJECTS)
	$(CXX) -shared -fPIC -o libgrabber.so $(OBJECTS)
	cp libgrabber.so ../lib

CGrabber.o:CGrabber.cpp CGrabber.h	
	$(CXX) -fPIC -o CGrabber.o -c CGrabber.cpp $(INCLUDES) $(FLAGS)
	cp CGrabber.h ../include

CGrabberController.o:CGrabberController.cpp CGrabberController.h CGrabber.o
	$(CXX) -fPIC -o CGrabberController.o -c CGrabberController.cpp $(INCLUDES) $(FLAGS)
	cp CGrabberController.h ../include
				
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
	if [ -f ../lib/libgrabber.so ]; then rm ../lib/libgrabber.so; fi;
