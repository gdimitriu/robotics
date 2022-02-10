SHELL = /bin/sh

INCLUDEPATHS = -I$(PREFIX_GPIO) -I../include
LIBS = -L$(PREFIX_GPIO) -L../lib -lpthread -lpigpio -lrt -ldroiddrivers

all: testPCA9685Servo CalibrationServoPCA9685 gps_receive_test
	
testPCA9685Servo : testPCA9685Servo.cpp 
	$(CXX) -fPIC -o testPCA9685Servo testPCA9685Servo.cpp $(INCLUDEPATHS) $(LIBPATHS) $(LIBS)
	cp testPCA9685Servo ../bin
	

CalibrationServoPCA9685 : CalibrationServoPCA9685.cpp
	$(CXX) -fPIC -o calibrationServoPCA9685 CalibrationServoPCA9685.cpp $(INCLUDEPATHS) $(LIBPATHS) $(LIBS)
	cp calibrationServoPCA9685 ../bin
	
gps_receive_test : gps_receive_test.cpp
	$(CXX) -fPIC -o gps_receive_test $(INCLUDEPATHS) $(LIBPATHS) $(LIBS) gps_receive_test.cpp -lm
	cp gps_receive_test ../bin
		
clean :
	if [ -f testPCA9685Servo ]; then rm testPCA9685Servo; fi;
	if [ -f ../bin/calibrationServoPCA9685 ]; then rm ../bin/calibrationServoPCA9685; fi;
	if [ -f calibrationServoPCA9685 ]; then rm calibrationServoPCA9685; fi;
	if [ -f ../bin/testPCA9685Servo ]; then rm ../bin/testPCA9685Servo; fi;	
	if [ -f gps_receive_test ]; then rm gps_receive_test; fi;
	if [ -f ../bin/gps_receive_test ]; then rm ../bin/gps_receive_test; fi;