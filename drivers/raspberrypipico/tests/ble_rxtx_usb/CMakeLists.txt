cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(ble_rxtx_usb)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(ble_rxtx_usb
        ble_rxtx_usb.c
        )

	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(ble_rxtx_usb pico_stdlib)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(ble_rxtx_usb 1)
    pico_enable_stdio_uart(ble_rxtx_usb 0)

	# create map/bin/hex file etc.
	pico_add_extra_outputs(ble_rxtx_usb)

	# add url via pico_set_program_url
#	example_auto_set_url(ble_rxtx_usb)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building interract_usb because TinyUSB submodule is not initialized in the SDK")
endif()
	
