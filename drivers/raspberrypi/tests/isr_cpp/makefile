SHELL = /bin/sh

LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt
INCLUDES = -I$(PREFIX_GPIO)
OBJECTS = CIsr.o
EXECUTABLES =  trigger_gpio_cpp

all: $(EXECUTABLES)

trigger_gpio_cpp: trigger_gpio.cpp CIsr.o
	$(CXX) -fPIC -o trigger_gpio_cpp trigger_gpio.cpp CIsr.o $(INCLUDES) $(LIBS)
	
CIsr.o:CIsr.cpp CIsr.h
	$(CXX) -fPIC -o CIsr.o -c CIsr.cpp $(INCLUDES)

clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
