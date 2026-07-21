cmake_minimum_required(VERSION 3.5)
project(led_with_gnu_assembly LANGUAGES C ASM)

add_executable(${PROJECT_NAME} main.s)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-nostartfiles -Ttext=0x87800000"
    SUFFIX ".elf"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND $ENV{CROSS_COMPILE}objcopy -O binary -j .text -j .data -j .rodata -S -g $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND imxdownload ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin ${PROJECT_NAME}.imx
    COMMAND $ENV{CROSS_COMPILE}objdump -D $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dis
    COMMENT "Converting ${PROJECT_NAME}.bin, ${PROJECT_NAME}.imx and generating disassembly"
)
