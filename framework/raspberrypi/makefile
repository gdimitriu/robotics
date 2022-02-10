SHELL = /bin/sh

ifeq (arm-linux,$(TOOLCHAIN_TYPE))
	CC := arm-linux-gnueabihf-gcc
	CXX := arm-linux-gnueabihf-g++
	AR := arm-linux-gnueabihf-ar
	OS_TYPE := raspberry
else ifeq(raspberry,$(TOOLCHAIN_TYPE))
	CC := gcc
	CXX := g++
	AR := ar
	OS_TYPE := raspberry
endif
export CC
export CXX
export AR
export OS_TYPE
all:
	cd loggers && $(MAKE)
	cd settings && $(MAKE)
	cd control_sensors && $(MAKE)
	cd control_engines && $(MAKE)
	cd grabber && $(MAKE)
	cd camera && $(MAKE)
	cd droid && $(MAKE)
clean:
	cd loggers && $(MAKE) clean
	cd settings && $(MAKE) clean	
	cd control_sensors && $(MAKE) clean
	cd control_engines && $(MAKE) clean
	cd grabber && $(MAKE) clean
	cd camera && $(MAKE) clean
	cd droid && $(MAKE) clean
	