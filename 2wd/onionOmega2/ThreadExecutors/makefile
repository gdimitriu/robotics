CC = mipsel-openwrt-linux-g++
INCLUDEPATHS = -I./

all: threadexecutors
	

threadexecutors : ThreadExecutor.o ThreadExecutors.o 
	$(CC) -shared -fPIC -o libthreadexecutors.so ThreadExecutors.o ThreadExecutor.o
	cp libthreadexecutors.so ../lib
	cp ThreadExecutors.h ../include
	cp ThreadExecutor.h ../include
	
ThreadExecutors.o : ThreadExecutors.cpp ThreadExecutors.h
	$(CC) -fPIC -c $(INCLUDEPATHS) ThreadExecutors.cpp
	
ThreadExecutor.o : ThreadExecutor.cpp ThreadExecutor.h
	$(CC) -fPIC -c $(INCLUDEPATHS) ThreadExecutor.cpp ThreadExecutor.h
    	
clean :
	if [ -f ThreadExecutors.o ]; then rm ThreadExecutors.o; fi;
	if [ -f ThreadExecutor.o ]; then rm ThreadExecutor.o; fi;
	if [ -f ThreadExecutor.h.gch ]; then rm ThreadExecutor.h.gch; fi;
	if [ -f libthreadexecutors.so ]; then rm libthreadexecutors.so; fi;
	if [ -f ../lib/libthreadexecutors.so ]; then rm ../lib/libthreadexecutors.so; fi;
	if [ -f ../include/ThreadExecutors.h ]; then rm ../include/ThreadExecutors.h; fi;
	if [ -f ../include/ThreadExecutor.h ]; then rm ../include/ThreadExecutor.h; fi;
 
