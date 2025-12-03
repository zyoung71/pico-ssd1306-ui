cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

pico_sdk_init()

project(pico-ssd1306-ui-impl C CXX ASM)

add_subdirectory(../ build)

add_executable(${CMAKE_PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    include
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_stdlib
    pico-ssd1306-ui
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})