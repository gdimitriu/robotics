SHELL = /bin/sh

LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt
INCLUDES = -I$(PREFIX_GPIO)
OBJECTS = 
EXECUTABLES = led_gpio detecti2c trigger_gpio ble_serial_receive ble_serial_sndrecv ble_serial_send hc-sr04 esp01receive testBleFile
FLAGS = -std=gnu++11 -ggdb

all: $(EXECUTABLES)
	cd isr_cpp && $(MAKE)
	cd camera && $(MAKE)
	cd mpu6050 && $(MAKE)
	cd libcamera && $(MAKE)
	cd MFRC522 && $(MAKE)

led_gpio:led_gpio.c
	$(CC) -fPIC -o led_gpio led_gpio.c $(LIBS) $(INCLUDES)

detecti2c:detecti2c.c
	$(CC) -fPIC -o detecti2c detecti2c.c $(LIBS) $(INCLUDES)

trigger_gpio:trigger_gpio.c
	$(CC) -fPIC -o trigger_gpio trigger_gpio.c $(LIBS) $(INCLUDES)

ble_serial_receive:ble_serial_receive.c
	$(CC) -fPIC -o ble_serial_receive ble_serial_receive.c $(LIBS) $(INCLUDES)
	
ble_serial_sndrecv:ble_serial_sndrecv.c
	$(CC) -fPIC -o ble_serial_sndrecv ble_serial_sndrecv.c $(LIBS) $(INCLUDES)	

ble_serial_send:ble_serial_send.c
	$(CC) -fPIC -o ble_serial_send ble_serial_send.c $(LIBS) $(INCLUDES)

hc-sr04:hc-sr04.cpp
	arm-linux-gnueabihf-g++ -fPIC -o hc-sr04 hc-sr04.cpp $(LIBS) $(INCLUDES) -lm -std=gnu++11 

esp01receive: esp01receive.c
	$(CC) -fPIC -o esp01receive esp01receive.c $(LIBS) $(INCLUDES)

testBleFile:testBleFile.c
	$(CC) -fPIC -o testBleFile testBleFile.c $(LIBS) $(INCLUDES)
	
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	cd isr_cpp && $(MAKE) clean
	cd camera && $(MAKE) clean
	cd mpu6050 && $(MAKE) clean
	cd MFRC522 && $(MAKE) clean
