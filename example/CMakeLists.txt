cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

pico_sdk_init()

project(pico-ssd1306-ui-impl C CXX ASM)
project(pico-ssd1306-ui-rladder C CXX ASM)

add_subdirectory(../ build)
add_subdirectory(lib/pico-rtc)

add_executable(${CMAKE_PROJECT_NAME}
src/main.cpp
)

add_executable(pico-ssd1306-ui-rladder-impl
src/main_rladder.cpp
src/components/SettingsCategoryComponent.cpp
src/components/ClockComponent.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
include
)

target_include_directories(pico-ssd1306-ui-rladder-impl PUBLIC
include
)

target_link_libraries(${CMAKE_PROJECT_NAME}
pico_stdlib
pico-ssd1306-ui
)

target_link_libraries(pico-ssd1306-ui-rladder-impl
pico_stdlib
pico-ssd1306-ui
hardware_watchdog
pico-rtc
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-fno-exceptions")

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

target_compile_options(pico-ssd1306-ui-rladder-impl PRIVATE "-fno-exceptions")

pico_enable_stdio_usb(pico-ssd1306-ui-rladder-impl 1)
pico_enable_stdio_uart(pico-ssd1306-ui-rladder-impl 1)
pico_add_extra_outputs(pico-ssd1306-ui-rladder-impl)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/tmp/upload.sh
"#!/bin/bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c \"adapter speed 5000\" -c \"program ${CMAKE_PROJECT_NAME}.elf verify reset exit\"
")

file(
    COPY ${CMAKE_CURRENT_BINARY_DIR}/tmp/upload.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/tmp/upload_rladder.sh
"#!/bin/bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c \"adapter speed 5000\" -c \"program pico-ssd1306-ui-rladder-impl.elf verify reset exit\"
")

file(
    COPY ${CMAKE_CURRENT_BINARY_DIR}/tmp/upload_rladder.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)