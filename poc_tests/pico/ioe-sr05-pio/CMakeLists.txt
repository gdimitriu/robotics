cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

project(ioe_sr05)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)

	add_executable(ioe_sr05
        ioe-sr05.c
	)

	pico_generate_pio_header(ioe_sr05 ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)

	target_link_libraries(ioe_sr05
			pico_stdlib
			pico_printf
			hardware_pio
	)
	# enable usb output, disable uart output
    pico_enable_stdio_usb(ioe_sr05 1)
	pico_enable_stdio_uart(ioe_sr05 0)
    
	pico_add_extra_outputs(ioe_sr05)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04 because TinyUSB submodule is not initialized in the SDK")
endif()

