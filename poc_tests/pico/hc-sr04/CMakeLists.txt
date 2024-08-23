cmake_minimum_required(VERSION 3.5)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(hc-sr04 C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(hc-sr04
        hc-sr04.cpp
        )
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(hc-sr04 pico_stdlib hardware_pio)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(hc-sr04 1)
    pico_enable_stdio_uart(hc-sr04 0)


    pico_set_binary_type(hc-sr04 no_flash)
	# create map/bin/hex file etc.
	pico_add_extra_outputs(hc-sr04)

	# add url via pico_set_program_url
#	example_auto_set_url(sr04_lcd)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04 because TinyUSB submodule is not initialized in the SDK")
endif()
	
