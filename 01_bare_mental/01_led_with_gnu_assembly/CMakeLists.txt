cmake_minimum_required(VERSION 3.5)

project(led LANGUAGES C ASM)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

add_executable(led main.s)
set_target_properties(led PROPERTIES
    LINK_FLAGS "-nostartfiles -Ttext=0x87800000"
    SUFFIX ".elf"
)

add_custom_command(TARGET led POST_BUILD
    COMMAND $ENV{CROSS_COMPILE}objcopy -O binary -j .text -j .data -j .rodata -S -g $<TARGET_FILE:led> ${CMAKE_BINARY_DIR}/led.bin
    COMMAND imxdownload ${CMAKE_BINARY_DIR}/led.bin led.imx
    COMMAND $ENV{CROSS_COMPILE}objdump -D $<TARGET_FILE:led> > ${CMAKE_BINARY_DIR}/led.dis
    COMMENT "Converting led.bin, led.imx and generating disassembly"
)
