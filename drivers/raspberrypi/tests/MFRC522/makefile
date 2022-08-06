SHELL = /bin/sh
ifeq (arm-linux,$(TOOLCHAIN_TYPE))
	CC := arm-linux-gnueabihf-gcc
	CXX := arm-linux-gnueabihf-g++
	AR := arm-linux-gnueabihf-ar
	LD := arm-linux-gnueabihf-ld
	OS_TYPE := raspberry
else ifeq (raspberry,$(TOOLCHAIN_TYPE))
	CC := gcc
	CXX := g++
	AR := ar
	OS_TYPE := raspberry
endif

export CC
export CXX
export AR
export OS_TYPE

LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt -lm -ldroiddrivers  -L../../lib
INCLUDES = -I$(PREFIX_GPIO) -I../../include
OBJECTS = 
EXECUTABLES = ReadData
FLAGS = -std=gnu++11 -ggdb
all: $(EXECUTABLES)

ReadData: ReadData.cpp
	$(CXX) -fPIC -o ReadData ReadData.cpp $(LIBS) $(INCLUDES) $(FLAGS)
	
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
