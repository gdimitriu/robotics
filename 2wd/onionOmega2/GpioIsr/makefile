CC = mipsel-openwrt-linux-g++
INCLUDEPATHS = -I./ -I../include

all: gpioisr
	cd examples && make
	

gpioisr : GpioIsr.o GpioIsrObserver.o
	$(CC) -shared -fPIC -o libgpioisr.so GpioIsr.o GpioIsrObserver.o
	cp libgpioisr.so ../lib
	cp GpioIsr.h ../include
	cp GpioIsrObserver.h ../include

GpioIsrObserver.o : GpioIsrObserver.cpp GpioIsrObserver.h
	$(CC) -fPIC -c $(INCLUDEPATHS) GpioIsrObserver.cpp
	
GpioIsr.o : GpioIsr.cpp GpioIsr.h GpioIsrObserver.h
	$(CC) -fPIC -c $(INCLUDEPATHS) GpioIsr.cpp
	
clean :
	if [ -f GpioIsr.o ]; then rm GpioIsr.o; fi;
	if [ -f GpioIsrObserver.o ]; then rm GpioIsrObserver.o; fi;
	if [ -f  libgpioisr.so ]; then rm libgpioisr.so; fi;
	if [ -f ../lib/libgpioisr.so ]; then rm ../lib/libgpioisr.so; fi;
	if [ -f ../include/GpioIsr.h ]; then rm ../include/GpioIsr.h; fi;
	if [ -f ../include/GpioIsrObserver.h ]; then rm ../include/GpioIsrObserver.h; fi;
	cd examples && make clean
 
