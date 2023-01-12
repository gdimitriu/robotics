cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
include(pico_libs_import.cmake)

project(hc-sr04_lcd C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(hc-sr04_lcd
        hc-sr04_lcd.cpp
        )
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(hc-sr04_lcd pico_stdlib hardware_pio ili934x hardware_spi)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(hc-sr04_lcd 1)
    pico_enable_stdio_uart(hc-sr04_lcd 0)


    
	# create map/bin/hex file etc.
	pico_add_extra_outputs(hc-sr04_lcd)

	# add url via pico_set_program_url
#	example_auto_set_url(sr04_lcd)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04_lcd because TinyUSB submodule is not initialized in the SDK")
endif()
	
