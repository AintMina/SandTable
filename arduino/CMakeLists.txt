cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/pico-sdk)
include(pico_sdk_import.cmake)

project(pico-projects)

pico_sdk_init()

add_library(leds lib/leds/src/leds.cpp)
target_link_libraries(leds pico_stdlib hardware_pwm)

add_library(driver lib/driver/src/driver.cpp)
target_link_libraries(driver pico_stdlib hardware_pwm)

add_library(angles lib/angles/src/angles.cpp)

add_executable(main src/main.cpp)

target_include_directories(main PUBLIC lib/leds/src)
target_include_directories(main PUBLIC lib/driver/src)
target_include_directories(main PUBLIC lib/angles/src)
target_link_libraries(main 
						pico_stdlib 
						leds 
						driver 
						angles 
						hardware_pwm 
						hardware_gpio 
						pico_multicore 
						pico_time)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)