cmake_minimum_required(VERSION 3.5)
project(led_with_c LANGUAGES C ASM)

add_executable(led_with_c main.c startup.s)
set_target_properties(led_with_c PROPERTIES
    LINK_FLAGS "-nostartfiles -T${CMAKE_SOURCE_DIR}/imx6ull.lds -Wl,--build-id=none"
    SUFFIX ".elf"
)

add_custom_command(TARGET led_with_c POST_BUILD
    COMMAND $ENV{CROSS_COMPILE}objcopy -O binary -S -g $<TARGET_FILE:led_with_c> ${CMAKE_BINARY_DIR}/led_with_c.bin
    COMMAND imxdownload ${CMAKE_BINARY_DIR}/led_with_c.bin led_with_c.imx
    COMMAND $ENV{CROSS_COMPILE}objdump -D $<TARGET_FILE:led_with_c> > ${CMAKE_BINARY_DIR}/led_with_c.dis
    COMMENT "Converting led.bin, led.imx and generating disassembly"
)
