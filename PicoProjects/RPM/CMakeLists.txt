cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(rpm C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD waveshare_rp2040_zero)
pico_sdk_init()

add_executable(${PROJECT_NAME}
    rpm.cpp
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_i2c
    hardware_adc
    hardware_irq
    pico_i2c_slave
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})