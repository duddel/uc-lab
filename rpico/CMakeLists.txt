cmake_minimum_required(VERSION 3.13)

# note: this must happen before project()
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rpico-lab)

pico_sdk_init()

#
# blink example
# 
add_executable(blink
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_blink.c
)

target_link_libraries(blink
  pico_stdlib
)

# On the Pico W, the led is connected to the Wifi chip.
# We need to link pico_cyw43_arch_none to access the led.
# PICO_CYW43_SUPPORTED is set if PICO_BOARD==pico_w.
if (PICO_CYW43_SUPPORTED)
  target_link_libraries(blink
    pico_cyw43_arch_none
  )
endif()

pico_add_extra_outputs(blink)

#
# debounce example
#
add_executable(debounce
  ${CMAKE_CURRENT_SOURCE_DIR}/src/debounce.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_debounce.c
)

target_include_directories(debounce
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(debounce
  pico_stdlib
)

# On the Pico W, the led is connected to the Wifi chip.
# We need to link pico_cyw43_arch_none to access the led.
# PICO_CYW43_SUPPORTED is set if PICO_BOARD==pico_w.
if (PICO_CYW43_SUPPORTED)
  target_link_libraries(debounce
    pico_cyw43_arch_none
  )
endif()

pico_add_extra_outputs(debounce)

#
# ws2812 example
#
add_executable(ws2812
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_ws2812.c
)

pico_generate_pio_header(ws2812
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio
)

target_link_libraries(ws2812
  pico_stdlib
  hardware_pio
)

pico_add_extra_outputs(ws2812)

#
# usb serial example
#
# add_executable(serial
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/main_serial.c
# )

# target_link_libraries(serial
#   pico_stdlib
# )

# pico_enable_stdio_usb(serial 1)
# pico_enable_stdio_uart(serial 0)

# pico_add_extra_outputs(serial)
