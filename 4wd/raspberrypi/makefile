SHELL = /bin/sh

ifeq (arm-linux,$(TOOLCHAIN_TYPE))
	CC := arm-linux-gnueabihf-gcc
	CXX := arm-linux-gnueabihf-g++
	AR := arm-linux-gnueabihf-ar
	OS_TYPE := raspberry
else
	ifeq(raspberry,$(TOOLCHAIN_TYPE))
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
	cd manual_operations && $(MAKE)
clean:
	cd manual_operations && $(MAKE) clean
	