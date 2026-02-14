cmake_minimum_required(VERSION 3.5)
project(buzzer LANGUAGES C ASM)

add_executable(${PROJECT_NAME}
    project/main.c
    project/startup.s
    bsp/clk/bsp_clk.c
    bsp/delay/bsp_delay.c
    bsp/led/bsp_led.c
    bsp/buzzer/bsp_buzzer.c
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/bsp/clk
    ${CMAKE_SOURCE_DIR}/bsp/delay
    ${CMAKE_SOURCE_DIR}/bsp/led
    ${CMAKE_SOURCE_DIR}/bsp/buzzer
    ${CMAKE_SOURCE_DIR}/imx6ul
)
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
