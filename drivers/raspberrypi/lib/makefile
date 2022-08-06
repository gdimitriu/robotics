SHELL = /bin/sh

DRIVERS = ../obj/Adafruit_PWMServoDriver.o ../obj/VK2828U7G5LF.o ../obj/CMFRC522.o

all: libdroiddrivers

libdroiddrivers: $(DRIVERS)
	$(CXX) -shared -fPIC -o libdroiddrivers.so $(DRIVERS)
		
clean:
	for object in $(DRIVERS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	if [ -f libdroiddrivers.so ]; then rm libdroiddrivers.so; fi;