all:
	cd Adafruit_PWMServoDriver && make
	if [ -d VL53L0X ]; then rm -rf VL53L0X; fi;
	git clone https://github.com/bitbank2/VL53L0X.git
	cp makes/Makefile VL53L0X/
	cd VL53L0X && make
	cd VK2828U7G5LF && make
	cd lib && make
	cd examples && make
	cd tests && make

clean:
	cd Adafruit_PWMServoDriver && make clean
	cd VL53L0X && make clean
	rm -rf VL53L0X
	cd VK2828U7G5LF && make clean
	cd lib && make clean
	cd examples && make clean
	cd tests && make clean