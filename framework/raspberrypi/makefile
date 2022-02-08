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
	cd loggers && make
	cd settings && make
	cd control_sensors && make
	cd control_engines && make
	cd grabber && make
	cd camera && make	
	cd droid && make
clean:
	cd loggers && make clean
	cd settings && make clean	
	cd control_sensors && make clean
	cd control_engines && make clean
	cd grabber && make clean
	cd camera && make clean
	cd droid && make clean
	