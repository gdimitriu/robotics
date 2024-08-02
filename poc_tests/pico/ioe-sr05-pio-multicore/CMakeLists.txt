cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

project(ioe_sr05_muticore)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)

	add_executable(ioe_sr05_muticore
        ioe-sr05.c
	)

	pico_generate_pio_header(ioe_sr05_muticore ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)

	target_link_libraries(ioe_sr05_muticore
			pico_stdlib
			pico_printf
			hardware_pio
			pico_multicore
			pico_sync
	)
	# enable usb output, disable uart output
    pico_enable_stdio_usb(ioe_sr05_muticore 1)
	pico_enable_stdio_uart(ioe_sr05_muticore 0)
    
	pico_add_extra_outputs(ioe_sr05_muticore)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hc-sr04 because TinyUSB submodule is not initialized in the SDK")
endif()

