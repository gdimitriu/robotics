/*
===============================================================================================================
QMC5883LCompass.h
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
#ifndef _QMC5883LCOMPASS_h_ 
#define _QMC5883LCOMPASS_h_
#include <hardware/i2c.h>

	void init_QMC5883LCompass(i2c_inst_t *i2c, uint8_t address);
    void setMode_QMC5883LCompass(uint8_t mode, uint8_t odr, uint8_t rng, uint8_t osr);
	void setMagneticDeclination_QMC5883LCompass(int degrees, uint8_t minutes);
	void setSmoothing_QMC5883LCompass(uint8_t steps, bool adv);
	void calibrate_QMC5883LCompass();
	void setCalibration_QMC5883LCompass(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max);
	void setCalibrationOffsets_QMC5883LCompass(float x_offset, float y_offset, float z_offset);
	void setCalibrationScales_QMC5883LCompass(float x_scale, float y_scale, float z_scale);
    float getCalibrationOffset_QMC5883LCompass(uint8_t index);
	float getCalibrationScale_QMC5883LCompass(uint8_t index);
	void clearCalibration_QMC5883LCompass();
	void setReset_QMC5883LCompass();
    void read_QMC5883LCompass();
	int getX_QMC5883LCompass();
	int getY_QMC5883LCompass();
	int getZ_QMC5883LCompass();
	int getAzimuth_QMC5883LCompass();
	uint8_t getBearing_QMC5883LCompass(int azimuth);
	void getDirection_QMC5883LCompass(char* myArray, int azimuth);

#endif
