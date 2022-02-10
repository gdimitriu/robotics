SHELL = /bin/sh

LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt -lwiringPi -lm -L$(PREFIX_WIRINGPI)
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_WIRINGPI)
OBJECTS = mpu6050wiringpi.o mpu6050.o
EXECUTABLES = mpu6050wiringpi mpu6050

all: $(EXECUTABLES)

mpu6050wiringpi:mpu6050wiringpi.o
	$(CC) -fPIC -o mpu6050wiringpi mpu6050wiringpi.o $(LIBS)
	
mpu6050wiringpi.o:mpu6050wiringpi.c
	$(CC) -fPIC -c mpu6050wiringpi.c $(INCLUDES)

mpu6050:mpu6050.o
	$(CC) -fPIC -o mpu6050 mpu6050.o $(LIBS)
	
mpu6050.o:mpu6050.c
	$(CC) -fPIC -c mpu6050.c $(INCLUDES)
	
clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
