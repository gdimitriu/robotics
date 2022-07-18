SHELL = /bin/sh

CFLAGS=-fPIC -c -Wall -O2 -fPIC

all: libtof.a libtof.so

libtof.a: tof.o
	$(AR) -rc libtof.a tof.o
	cp libtof.a ../lib

libtof.so: tof.o
	$(CXX) -shared -o libtof.so tof.o
	cp libtof.so ../lib
	cp tof.h ../include
	
tof.o: tof.c tof.h
	$(CXX) $(CFLAGS) tof.c

clean:
	rm *.o libtof.a libtof.so
	rm ../lib/libtof.a
	rm ../lib/libtof.so
	rm ../include/tof.h
