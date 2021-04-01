cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(hello_gpio_irq)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)

	add_executable(hello_gpio_irq
        hello_gpio_irq.c
        )

	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(hello_gpio_irq pico_stdlib hardware_pwm)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(hello_gpio_irq 1)
    pico_enable_stdio_uart(hello_gpio_irq 0)

	# create map/bin/hex file etc.
	pico_add_extra_outputs(hello_gpio_irq)

	# add url via pico_set_program_url
#	example_auto_set_url(hello_gpio_irq)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building interract_usb because TinyUSB submodule is not initialized in the SDK")
endif()
	
