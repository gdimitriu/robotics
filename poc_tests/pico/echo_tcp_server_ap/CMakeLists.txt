cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(echo_tcp_server_ap)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(echo_tcp_server_ap
        picow_tcp_server.c
        dhcpserver/dhcpserver.c
        )
target_include_directories(echo_tcp_server_ap PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        )
target_link_libraries(echo_tcp_server_ap
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib pico_multicore
        )
pico_enable_stdio_usb(echo_tcp_server_ap 1)
pico_enable_stdio_uart(echo_tcp_server_ap 0)
pico_add_extra_outputs(echo_tcp_server_ap)

