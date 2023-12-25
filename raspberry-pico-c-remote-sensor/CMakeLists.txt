cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
project(raspberry_pico_c_remote_sensor C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(WIFI_SSID ${WIFI_SSID} CACHE INTERNAL "WiFi SSID")
set(WIFI_PASSWORD ${WIFI_PASSWORD} CACHE INTERNAL "WiFi password")
set(TCP_SERVER_IP ${TCP_SERVER_IP} CACHE INTERNAL "TCP server ip")
set(TCP_PORT ${TCP_PORT} CACHE INTERNAL "TCP server port")

message("Raspberry Pi Pico SDK version ${PICO_SDK_VERSION_STRING}")

pico_sdk_init()

add_executable(raspberry_pico_c_remote_sensor main.c src/dht.c src/tcp_utils.c src/sleep_utils.c)
target_include_directories(raspberry_pico_c_remote_sensor PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(raspberry_pico_c_remote_sensor
        pico_cyw43_arch_lwip_poll # pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        hardware_gpio
        hardware_sleep
)
target_compile_definitions(raspberry_pico_c_remote_sensor PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        TCP_SERVER_IP=\"${TCP_SERVER_IP}\"
        TCP_PORT=\"${TCP_PORT}\"
)

pico_enable_stdio_usb(raspberry_pico_c_remote_sensor 1)
pico_enable_stdio_uart(raspberry_pico_c_remote_sensor 0)

pico_add_extra_outputs(raspberry_pico_c_remote_sensor)