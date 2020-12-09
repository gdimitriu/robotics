CFLAGS=-fPIC -c -Wall -O2 -fPIC
CC=arm-linux-gnueabihf-g++

all: libtof.a libtof.so

libtof.a: tof.o
	$(AR) -rc libtof.a tof.o
	cp libtof.a ../lib

libtof.so: tof.o
	$(CC) -shared -o libtof.so tof.o
	cp libtof.so ../lib
	cp tof.h ../include
	
tof.o: tof.c tof.h
	$(CC) $(CFLAGS) tof.c

clean:
	rm *.o libtof.a libtof.so
	rm ../lib/libtof.a
	rm ../lib/libtof.so
	rm ../include/tof.h
