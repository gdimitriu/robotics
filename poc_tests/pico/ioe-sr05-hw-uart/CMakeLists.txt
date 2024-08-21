cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

project(ioe-sr05-hw-uart)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)

	add_executable(ioe-sr05-hw-uart
        ioe-sr05.c
	)

	target_link_libraries(ioe-sr05-hw-uart
			pico_stdlib
			pico_printf
			hardware_uart
	)
	# enable usb output, disable uart output
    pico_enable_stdio_usb(ioe-sr05-hw-uart 1)
	pico_enable_stdio_uart(ioe-sr05-hw-uart 0)
    
	pico_add_extra_outputs(ioe-sr05-hw-uart)
	
	pico_set_binary_type(ioe-sr05-hw-uart no_flash)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04 because TinyUSB submodule is not initialized in the SDK")
endif()

