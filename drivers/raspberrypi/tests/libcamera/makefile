SHELL = /bin/sh
LIBCAMERA_LIBS=-L$(PREFIX_LIBCAMERA)/lib/arm-linux-gnueabihf -lcamera_app -lcamera-base -lcamera -limages -lencoders -lpreview -loutputs -lepoxy -lpost_processing_stages
LIB_SYSTEM = -L$(PREFIX_SYSTEM)/lib -latomic -ljpeg -ltiff -lexif -lpng16 -ldrm -lgnutls  -llzma -ljbig -ldeflate -lz -lnettle -Xlinker --rpath-link=$(PREFIX_SYSTEM)/lib
BOOST_LIBS=-L$(PREFIX_BOOST)/lib -lboost_program_options
LIBS = -L$(PREFIX_GPIO) -lpthread -lpigpio -lrt $(LIBCAMERA_LIBS) $(BOOST_LIBS) $(LIB_SYSTEM)
INCLUDES = -I$(PREFIX_GPIO) -I$(PREFIX_LIBCAMERA)/include -I$(PREFIX_BOOST)/include
FLAGS = -std=c++17
OBJECTS =  
EXECUTABLES = libcamera_jpeg testCamera

all: $(EXECUTABLES)

libcamera_jpeg.o: libcamera_jpeg.cpp
	$(CXX) -fPIC -c -o libcamera_jpeg.o libcamera_jpeg.cpp $(INCLUDES) $(LIBS) $(FLAGS)

libcamera_jpeg: libcamera_jpeg.o
	$(CXX) -fPIC -o libcamera_jpeg libcamera_jpeg.o $(INCLUDES) $(LIBS) $(FLAGS)

testCamera: testCamera.cpp
	$(CXX) -fPIC -o testCamera testCamera.cpp $(INCLUDES) $(LIBS) $(FLAGS) 

clean:
	for object in $(EXECUTABLES);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
	for object in $(OBJECTS);\
	do \
		if [ -f $$object ]; then rm $$object; fi; \
	done
