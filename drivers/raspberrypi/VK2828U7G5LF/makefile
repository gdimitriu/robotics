all: VK2828U7G5LF.o
	
VK2828U7G5LF.o : VK2828U7G5LF.cpp VK2828U7G5LF.h
	$(CXX) -fPIC -c -I./ VK2828U7G5LF.cpp -I../include -o ../obj/VK2828U7G5LF.o
	cp VK2828U7G5LF.h ../include
	

clean :
	if [ -f ../obj/VK2828U7G5LF.o ]; then rm ../obj/VK2828U7G5LF.o; fi;
	if [ -f ../include/VK2828U7G5LF.h ]; then rm ../include/VK2828U7G5LF.h; fi; 
