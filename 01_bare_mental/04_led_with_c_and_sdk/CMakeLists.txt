cmake_minimum_required(VERSION 3.5)
project(led_with_c_and_sdk LANGUAGES C ASM)

add_executable(${PROJECT_NAME} main.c startup.s)
set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-nostartfiles -T${CMAKE_SOURCE_DIR}/imx6ull.lds -Wl,--build-id=none"
        SUFFIX ".elf"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $ENV{CROSS_COMPILE}objcopy -O binary -S -g $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
        COMMAND imxdownload ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin ${PROJECT_NAME}.imx
        COMMAND $ENV{CROSS_COMPILE}objdump -D $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dis
        COMMENT "Converting led.bin, led.imx and generating disassembly"
)
