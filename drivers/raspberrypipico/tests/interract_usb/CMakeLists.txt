cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(interract_usb)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
    add_executable(interract_usb
            interract_usb.c
            )

    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(interract_usb pico_stdlib)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(interract_usb 1)
    pico_enable_stdio_uart(interract_usb 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(interract_usb)

    # add url via pico_set_program_url
   # example_auto_set_url(interract_usb)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building interract_usb because TinyUSB submodule is not initialized in the SDK")
endif()
