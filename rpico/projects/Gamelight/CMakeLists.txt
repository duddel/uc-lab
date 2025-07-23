cmake_minimum_required(VERSION 3.13)

# note: this must happen before project()
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(gamelight)
pico_sdk_init()

# Root of the Pico Lab in this repository (to use shared code)
set(RPICO_LAB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(${CMAKE_PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

pico_generate_pio_header(${CMAKE_PROJECT_NAME}
  ${RPICO_LAB_ROOT}/src/ws2812.pio
)

target_include_directories(${CMAKE_PROJECT_NAME}
  PRIVATE ${RPICO_LAB_ROOT}/src
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  pico_stdlib
  hardware_pio
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
