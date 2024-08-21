cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

project(ioe-sr05-hw-uart-multicore)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)

	add_executable(ioe-sr05-hw-uart-multicore
        ioe-sr05.c
	)


	target_link_libraries(ioe-sr05-hw-uart-multicore
			pico_stdlib
			pico_printf
			hardware_pio
			pico_multicore
			pico_sync
			hardware_uart
	)
	# enable usb output, disable uart output
    pico_enable_stdio_usb(ioe-sr05-hw-uart-multicore 1)
	pico_enable_stdio_uart(ioe-sr05-hw-uart-multicore 0)
    
	pico_add_extra_outputs(ioe-sr05-hw-uart-multicore)
	
	pico_set_binary_type(ioe-sr05-hw-uart-multicore no_flash)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04 because TinyUSB submodule is not initialized in the SDK")
endif()

