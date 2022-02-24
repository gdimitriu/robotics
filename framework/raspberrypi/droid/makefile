SHELL = /bin/sh

INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_DRIVERS)/include -I../include
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
