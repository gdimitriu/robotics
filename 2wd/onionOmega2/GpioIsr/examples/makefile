CC = mipsel-openwrt-linux-g++
INCLUDEPATHS = -I./ -I../../include -I$(PREFIX_FAST_GPIO)/include -I$(PREFIX_I2C_EXP_DRIVER)/include -I$(PREFIX_DRIVERS)/include
LIBPATHS = -L../../lib -L$(PREFIX_I2C_EXP_DRIVER)/lib -L$(PREFIX_DRIVERS)/lib
LIBS = -lthreadexecutors -lsx1509 -lonioni2c -loniondebug -lgpioisr

all: led_on_off_irq_threads led_on_off_irq multiple_irq_threads
	

led_on_off_irq_threads : led_on_off_irq_threads.cpp ExGpioIsrObserver.cpp ExGpioIsrObserver.h
	$(CC) -fPIC -o led_on_off_irq_threads $(INCLUDEPATHS) $(LIBPATHS) $(LIBS) led_on_off_irq_threads.cpp ExGpioIsrObserver.cpp $(PREFIX_FAST_GPIO)/src/fastgpioomega2.cpp $(PREFIX_FAST_GPIO)/src/module.cpp
led_on_off_irq : led_on_off_irq_threads.cpp ExGpioIsrObserver.cpp ExGpioIsrObserver.h
	$(CC) -fPIC -o led_on_off_irq $(INCLUDEPATHS) $(LIBPATHS) $(LIBS) led_on_off_irq_threads.cpp ExGpioIsrObserver.cpp $(PREFIX_FAST_GPIO)/src/fastgpioomega2.cpp $(PREFIX_FAST_GPIO)/src/module.cpp
	
multiple_irq_threads: multiple_irq_threads.cpp ExGpioIsrObserverPrint.cpp ExGpioIsrObserverPrint.h
	$(CC) -fPIC -o multiple_irq_threads $(INCLUDEPATHS) $(LIBPATHS) $(LIBS) multiple_irq_threads.cpp ExGpioIsrObserverPrint.cpp
clean :
	if [ -f led_on_off_irq_threads ]; then rm led_on_off_irq_threads; fi;
	if [ -f led_on_off_irq ]; then rm led_on_off_irq; fi;
	if [ -f multiple_irq_threads ]; then rm multiple_irq_threads; fi;
  
