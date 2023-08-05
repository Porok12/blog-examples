cmake_minimum_required(VERSION 3.26)

include(pico_sdk_import.cmake)

project(raspberryp_pico_c_blinking_led C CXX ASM)

pico_sdk_init()


add_executable(raspberryp_pico_c_blinking_led main.c)

target_link_libraries(raspberryp_pico_c_blinking_led pico_stdlib)

pico_add_extra_outputs(raspberryp_pico_c_blinking_led)
