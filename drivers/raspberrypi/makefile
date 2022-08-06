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
	cp makes/Makefile.vk53l0x VL53L0X/Makefile
	cd VL53L0X && $(MAKE)
	if [ -d CMFRC522 ]; then rm -rf CMFRC522; fi;
	git clone https://github.com/chihebabid/CMFRC522.git
	cp makes/Makefile.cmfrc522 CMFRC522/Makefile
	cd CMFRC522 && $(MAKE)
	cd VK2828U7G5LF && $(MAKE)
	cd lib && $(MAKE)
	cd examples && $(MAKE)
	cd tests && $(MAKE)

clean:
	cd Adafruit_PWMServoDriver && $(MAKE) clean
	if [ -d VL53L0X ]; then \
		cd VL53L0X && $(MAKE) clean; \
		cd ..\
		rm -rf VL53L0X; \
	fi;
	if [ -d CMFRC522 ]; then \
		cd CMFRC522 && $(MAKE) clean; \
		cd ..\
		rm -rf CMFRC522;\
	fi;
	cd VK2828U7G5LF && $(MAKE) clean
	cd lib && $(MAKE) clean
	cd examples && $(MAKE) clean
	cd tests && $(MAKE) clean
