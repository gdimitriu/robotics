cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(timing)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(timing
        timing.c
        )
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(timing pico_stdlib pico_multicore)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(timing 1)
    pico_enable_stdio_uart(timing 0)

	# create map/bin/hex file etc.
	pico_add_extra_outputs(timing)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building timing because TinyUSB submodule is not initialized in the SDK")
endif()
	
