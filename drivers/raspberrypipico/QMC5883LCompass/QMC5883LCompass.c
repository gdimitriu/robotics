/*
===============================================================================================================
QMC5883LCompass.c
Library for using QMC5583L series chip boards as a compass.
Learn more at [https://github.com/mprograms/QMC5883LCompass]

Supports:

- Getting values of XYZ axis.
- Calculating Azimuth.
- Getting 16 point Azimuth bearing direction (0 - 15).
- Getting 16 point Azimuth bearing Names (N, NNE, NE, ENE, E, ESE, SE, SSE, S, SSW, SW, WSW, W, WNW, NW, NNW)
- Smoothing of XYZ readings via rolling averaging and min / max removal.
- Optional chipset modes (see below)

===============================================================================================================

v1.0 - June 13, 2019
Written by MPrograms 
Github: [https://github.com/mprograms/]

Release under the GNU General Public License v3
[https://www.gnu.org/licenses/gpl-3.0.en.html]

===============================================================================================================



FROM QST QMC5883L Datasheet [https://nettigo.pl/attachments/440]
-----------------------------------------------
 MODE CONTROL (MODE)
	Standby			0x00
	Continuous		0x01

OUTPUT DATA RATE (ODR)
	10Hz        	0x00
	50Hz        	0x04
	100Hz       	0x08
	200Hz       	0x0C

FULL SCALE (RNG)
	2G          	0x00
	8G          	0x10

OVER SAMPLE RATIO (OSR)
	512         	0x00
	256         	0x40
	128         	0x80
	64          	0xC0 
  
*/
/*
 * August 2024
 * Ported to raspberry pico by Gabriel Dimitriu
 */

#include <hardware/i2c.h>
#include <math.h>
#include "QMC5883LCompass.h"
#include <pico/printf.h>


#define PI 3.14159265359

static float _magneticDeclinationDegrees = 0;
static bool _smoothUse = false;
static uint8_t _smoothSteps = 5;
static bool _smoothAdvanced = false;
static uint8_t _ADDR = 0x0D;
static int _vRaw[3] = {0,0,0};
static int _vHistory[10][3];
static int _vScan = 0;
static long _vTotals[3] = {0,0,0};
static int _vSmooth[3] = {0,0,0};
	
static float _offset[3] = {0.,0.,0.};
static float _scale[3] = {1.,1.,1.};
static int _vCalibrated[3];

static i2c_inst_t *_i2c;

static const char _bearings[16][3] =  {
	{' ', ' ', 'N'},
	{'N', 'N', 'E'},
	{' ', 'N', 'E'},
	{'E', 'N', 'E'},
	{' ', ' ', 'E'},
	{'E', 'S', 'E'},
	{' ', 'S', 'E'},
	{'S', 'S', 'E'},
	{' ', ' ', 'S'},
	{'S', 'S', 'W'},
	{' ', 'S', 'W'},
	{'W', 'S', 'W'},
	{' ', ' ', 'W'},
	{'W', 'N', 'W'},
	{' ', 'N', 'W'},
	{'N', 'N', 'W'},
};

static void _smoothing() {
	uint8_t max = 0;
	uint8_t min = 0;
	
	if ( _vScan > _smoothSteps - 1 ) { _vScan = 0; }
	
	for ( int i = 0; i < 3; i++ ) {
		if ( _vTotals[i] != 0 ) {
			_vTotals[i] = _vTotals[i] - _vHistory[_vScan][i];
		}
		_vHistory[_vScan][i] = _vCalibrated[i];
		_vTotals[i] = _vTotals[i] + _vHistory[_vScan][i];
		
		if ( _smoothAdvanced ) {
			max = 0;
			for (int j = 0; j < _smoothSteps - 1; j++) {
				max = ( _vHistory[j][i] > _vHistory[max][i] ) ? j : max;
			}
			
			min = 0;
			for (int k = 0; k < _smoothSteps - 1; k++) {
				min = ( _vHistory[k][i] < _vHistory[min][i] ) ? k : min;
			}
					
			_vSmooth[i] = ( _vTotals[i] - (_vHistory[max][i] + _vHistory[min][i]) ) / (_smoothSteps - 2);
		} else {
			_vSmooth[i] = _vTotals[i]  / _smoothSteps;
		}
	}
	
	_vScan++;
}

static void _applyCalibration() {
	_vCalibrated[0] = (_vRaw[0] - _offset[0]) * _scale[0];
	_vCalibrated[1] = (_vRaw[1] - _offset[1]) * _scale[1];
	_vCalibrated[2] = (_vRaw[2] - _offset[2]) * _scale[2];
}

static void _writeReg(uint8_t reg, uint8_t val) {
	int ret;
	uint8_t buffer[2] = {reg, val};
	ret = i2c_write_blocking(_i2c,_ADDR, buffer, 2, false);
	if ( ret != PICO_ERROR_GENERIC ) {
		printf("Write %#0x to %#0x with status %d\n",reg, val,ret);
	} else {
		printf("Could not write to %#0x \n", reg);
	}
}

static int _get(int index) {
	if ( _smoothUse ) 
		return _vSmooth[index];
	
	return _vCalibrated[index];
}

static uint8_t _getStatus() {
	uint8_t ready;
	uint8_t reg = 0x06;
	int ret;
	ret = i2c_write_blocking(_i2c,_ADDR, &reg, 1, true);
	if ( ret != PICO_ERROR_GENERIC ) {
		ret = i2c_read_blocking(_i2c, _ADDR, &ready, 1, false);
		return ready;
	}
	return 0;
}

void init_QMC5883LCompass(i2c_inst_t *i2c, uint8_t address) {
	_i2c = i2c;
	_ADDR = address;
	_writeReg(0x0B,0x01);
	setMode_QMC5883LCompass(0x01,0x0C,0x10,0X00);
	uint8_t ready = _getStatus();
	int count = 0;
	while( (ready & 0x01) !=  0x01 ) {
		ready = _getStatus();
		sleep_ms(100);
		if ( count % 10 ) {
			printf("ready = %0x\n",ready);
			count = 1;
		}
		count++;
	}
	printf("device ready %0x\n",ready);
}

void setMode_QMC5883LCompass(uint8_t mode, uint8_t odr, uint8_t rng, uint8_t osr) {
	_writeReg(0x09,mode|odr|rng|osr);	
}

void setMagneticDeclination_QMC5883LCompass(int degrees, uint8_t minutes) {
	_magneticDeclinationDegrees = degrees + minutes / 60;
}

void setSmoothing_QMC5883LCompass(uint8_t steps, bool adv) {
	_smoothUse = true;
	_smoothSteps = ( steps > 10) ? 10 : steps;
	_smoothAdvanced = (adv == true) ? true : false;
}

void calibrate_QMC5883LCompass() {
	clearCalibration_QMC5883LCompass();
	long calibrationData[3][2] = {{65000, -65000}, {65000, -65000}, {65000, -65000}};
  	long	x = calibrationData[0][0] = calibrationData[0][1] = getX_QMC5883LCompass();
  	long	y = calibrationData[1][0] = calibrationData[1][1] = getY_QMC5883LCompass();
  	long	z = calibrationData[2][0] = calibrationData[2][1] = getZ_QMC5883LCompass();

	absolute_time_t startTime = get_absolute_time();

	while((get_absolute_time() - startTime) < 10000) {
		read_QMC5883LCompass();

  		x = getX_QMC5883LCompass();
  		y = getY_QMC5883LCompass();
  		z = getZ_QMC5883LCompass();

		if(x < calibrationData[0][0]) {
			calibrationData[0][0] = x;
		}
		if(x > calibrationData[0][1]) {
			calibrationData[0][1] = x;
		}

		if(y < calibrationData[1][0]) {
			calibrationData[1][0] = y;
		}
		if(y > calibrationData[1][1]) {
			calibrationData[1][1] = y;
		}

		if(z < calibrationData[2][0]) {
			calibrationData[2][0] = z;
		}
		if(z > calibrationData[2][1]) {
			calibrationData[2][1] = z;
		}
	}

	setCalibration_QMC5883LCompass(
		calibrationData[0][0],
		calibrationData[0][1],
		calibrationData[1][0],
		calibrationData[1][1],
		calibrationData[2][0],
		calibrationData[2][1]
	);
}

void setCalibration_QMC5883LCompass(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
	setCalibrationOffsets_QMC5883LCompass(
		(x_min + x_max)/2,
		(y_min + y_max)/2,
		(z_min + z_max)/2
	);

	float x_avg_delta = (x_max - x_min)/2;
	float y_avg_delta = (y_max - y_min)/2;
	float z_avg_delta = (z_max - z_min)/2;

	float avg_delta = (x_avg_delta + y_avg_delta + z_avg_delta) / 3;

	setCalibrationScales_QMC5883LCompass(
		avg_delta / x_avg_delta,
		avg_delta / y_avg_delta,
		avg_delta / z_avg_delta
	);
}

void setCalibrationOffsets_QMC5883LCompass(float x_offset, float y_offset, float z_offset) {
	_offset[0] = x_offset;
	_offset[1] = y_offset;
	_offset[2] = z_offset;
}

void setCalibrationScales_QMC5883LCompass(float x_scale, float y_scale, float z_scale) {
	_scale[0] = x_scale;
	_scale[1] = y_scale;
	_scale[2] = z_scale;
}

float getCalibrationOffset_QMC5883LCompass(uint8_t index) {
	return _offset[index];
}

float getCalibrationScale_QMC5883LCompass(uint8_t index) {
	return _scale[index];
}

void clearCalibration_QMC5883LCompass() {
	setCalibrationOffsets_QMC5883LCompass(0., 0., 0.);
	setCalibrationScales_QMC5883LCompass(1., 1., 1.);
}

void setReset_QMC5883LCompass() {
	_writeReg(0x0A,0x80);
}

void read_QMC5883LCompass() {
	if ( ( _getStatus() & 0x01) != 0x01 ) {
		return;
	}
	uint8_t reg = 0x00;
	int err = i2c_write_blocking(_i2c,_ADDR, &reg, 1, false);
	if (err != PICO_ERROR_GENERIC) {
		uint8_t buffer[6];
		int ret = i2c_read_blocking(_i2c, _ADDR, buffer, 6, false);
		if ( ret == PICO_ERROR_GENERIC ) {
			printf("Unable to read data\n");
		} else {
			printf("Read %d data %#0x %#0x %#0x %#0x %#0x %#0x\n", ret, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
		}
		
		_vRaw[0] = buffer[0] + buffer[1] * 256;
		_vRaw[1] = buffer[2] + buffer[3] * 256;
		_vRaw[2] = buffer[4] + buffer[5] * 256;

		_applyCalibration();
		
		if ( _smoothUse ) {
			_smoothing();
		}
		
		//uint8_t overflow = Wire.read() & 0x02;
		//return overflow << 2;
	} else {
		printf("Eroror write 0x00\n");
	}
}

int getX_QMC5883LCompass() {
	return _get(0);
}

int getY_QMC5883LCompass() {
	return _get(1);
}

int getZ_QMC5883LCompass() {
	return _get(2);
}

int getAzimuth_QMC5883LCompass() {
	float heading = atan2( getY_QMC5883LCompass(), getX_QMC5883LCompass() ) * 180.0 / PI;
	heading += _magneticDeclinationDegrees;
	return (int)heading % 360;
}

uint8_t getBearing_QMC5883LCompass(int azimuth) {
	unsigned long a = ( azimuth > -0.5 ) ? azimuth / 22.5 : (azimuth+360)/22.5;
	unsigned long r = a - (int)a;
	uint8_t sexdec = 0;	
	sexdec = ( r >= .5 ) ? ceil(a) : floor(a);
	return sexdec;
}

void getDirection_QMC5883LCompass(char* myArray, int azimuth) {
	int d = getBearing_QMC5883LCompass(azimuth);
	myArray[0] = _bearings[d][0];
	myArray[1] = _bearings[d][1];
	myArray[2] = _bearings[d][2];
}
