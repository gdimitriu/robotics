cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(QMC5883LCompass C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(QMC5883LCompass
		main.c
        QMC5883LCompass.c
        )
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(QMC5883LCompass pico_stdlib hardware_i2c hardware_timer)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(QMC5883LCompass 1)
    pico_enable_stdio_uart(QMC5883LCompass 0)


    pico_set_binary_type(QMC5883LCompass no_flash)
	# create map/bin/hex file etc.
	pico_add_extra_outputs(QMC5883LCompass)

	# add url via pico_set_program_url
elseif(PICO_ON_DEVICE)
    message(WARNING "not building QMC5883LCompass because TinyUSB submodule is not initialized in the SDK")
endif()
	
