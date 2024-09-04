#include <hardware/gpio.h>
#include <pico/printf.h>
#include <pico/stdlib.h>
#include "pico/binary_info.h"
#include "QMC5883LCompass.h"

int main() {
	// Enable UART so we can print status output
	stdio_init_all();
	#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
		#warning i2c/bus_scan example requires a board with I2C pins
		puts("Default I2C pins were not defined");
	#else
		// This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
		i2c_init(i2c_default, 400 * 1000);
		gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
		gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
		gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
		gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
		// Make the I2C pins available to picotool
		bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
		getchar();
		init_QMC5883LCompass(i2c_default, 0x0D);
		calibrate_QMC5883LCompass();
		setCalibrationOffsets_QMC5883LCompass(getCalibrationOffset_QMC5883LCompass(0), getCalibrationOffset_QMC5883LCompass(1), getCalibrationOffset_QMC5883LCompass(2));
		setCalibrationScales_QMC5883LCompass(getCalibrationScale_QMC5883LCompass(0), getCalibrationScale_QMC5883LCompass(1), getCalibrationScale_QMC5883LCompass(2));
		getchar();
		while( 1) {
			read_QMC5883LCompass();
			int azimuth = getAzimuth_QMC5883LCompass();
			printf("Azimuth %d\n",azimuth);
			sleep_ms(5);
		}
	#endif
}
