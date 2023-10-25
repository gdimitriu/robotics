cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(hc-sr04_multicore_queue C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(hc-sr04_multicore_queue
        hc-sr04_multicore_queue.cpp
        )
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(hc-sr04_multicore_queue pico_stdlib pico_multicore hardware_pio )

	# enable usb output, disable uart output
    pico_enable_stdio_usb(hc-sr04_multicore_queue 1)
    pico_enable_stdio_uart(hc-sr04_multicore_queue 0)


    
	# create map/bin/hex file etc.
	pico_add_extra_outputs(hc-sr04_multicore_queue)

	# add url via pico_set_program_url
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04_multicore_queue because TinyUSB submodule is not initialized in the SDK")
endif()
	
