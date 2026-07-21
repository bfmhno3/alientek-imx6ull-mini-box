.global _start @ 全局符号

/**
 * 定义与 GPIO1_IO03 相关的寄存器地址
 */
.equ CCM_CCGR1,                         0x020c406c
.equ IOMUX_SW_MUX_CTL_PAD_GPIO1_IO03,   0x020e0068
.equ IOMUX_SW_PAD_CTL_PAD_GPIO1_IO03,   0x020e02f4
.equ GPIO1_GDIR,                        0x0209c004 @ GPIO1 方向寄存器地址
.equ GPIO1_DR,                          0x0209c000 @ GPIO1 数据寄存器地址

_start: @ 程序入口
    /**
     * 1. 使能 GPIO1 时钟
     */
    ldr r0, =CCM_CCGR1
    ldr r1, =0xffffffff @ 0b00111100 00000000 00000000 # 使能 GPIO1 时钟
    str r1, [r0]

    /**
     * 2. 配置 GPIO1_IO03 为 GPIO 模式
     */
    ldr r0, =IOMUX_SW_MUX_CTL_PAD_GPIO1_IO03
    ldr r1, =0x5 @ 设置为 GPIO 模式
    str r1, [r0]

    /**
     * 3. 设置 GPIO1_IO03 为输出模式
     */
    ldr r0, =IOMUX_SW_PAD_CTL_PAD_GPIO1_IO03
    ldr r1, =0x10b0 @ 配置为输出模式，无上下拉，快速响应
    str r1, [r0]

    /**
     * 4. 设置 GPIO1_IO03 为输出模式
     */
    ldr r0, =GPIO1_GDIR
    ldr r1, =0x8 @ 设置 GPIO1_IO03（对应位3）为输出模式
    str r1, [r0] @ 写回 GPIO1 方向寄存器

    /**
     * 5. 点亮 LED（输出低电平）
     */
    ldr r0, =GPIO1_DR @ GPIO1 数据寄存器地址
    ldr r1, =0x0 @ 设置 GPIO1_IO03（对应位3）为低电平
    str r1, [r0] @ 写回 GPIO1 数据寄存器

loop:
    b loop @ 无限循环，保持 LED 点亮状态
