SHELL = /bin/sh

ifeq (arm-linux,$(TOOLCHAIN_TYPE))
	CC := arm-linux-gnueabihf-gcc
	CXX := arm-linux-gnueabihf-g++
	AR := arm-linux-gnueabihf-ar
	LD := arm-linux-gnueabihf-ld
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
	cd Adafruit_PWMServoDriver && $(MAKE)
	if [ -d VL53L0X ]; then rm -rf VL53L0X; fi;
	git clone https://github.com/bitbank2/VL53L0X.git
	cp makes/Makefile VL53L0X/
	cd VL53L0X && $(MAKE)
	cd VK2828U7G5LF && $(MAKE)
	cd lib && $(MAKE)
	cd examples && $(MAKE)
	cd tests && $(MAKE)

clean:
	cd Adafruit_PWMServoDriver && $(MAKE) clean
	cd VL53L0X && $(MAKE) clean
	rm -rf VL53L0X
	cd VK2828U7G5LF && $(MAKE) clean
	cd lib && $(MAKE) clean
	cd examples && $(MAKE) clean
	cd tests && $(MAKE) clean