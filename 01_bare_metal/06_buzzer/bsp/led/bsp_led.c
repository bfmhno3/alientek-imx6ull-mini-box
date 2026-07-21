#include "bsp_led.h"

#include "imx6ul.h"

/**
 * 初始化 LED 对应的 GPIO
 */
void led_init(void)
{
    // 1. 初始化 IO 复用，复用为 GPIO1_IO03
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);

    /**
     * 2. 配置 GPIO1_IO03 的电气属性
     *
     * bit [16,  0] : HYS 关闭
     * bit [15, 14] : 00 默认下拉
     * bit [13]     : keeper 功能
     * bit [12]     : 1 pull/keeper 使能
     * bit [11]     : 0 关闭开路输出
     * bit [ 7, 6]  : 10 速度 100MHz
     * bit [ 5, 3]  : 110 R0/6 驱动能力
     * bit [0]      : 0 低转换率
     */
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x10b0);

    // 3. 初始化 GPIO，GPIO1_IO03 设置为输出
    GPIO1->GDIR |= (1 << 3);

    // 4. 设置 GPIO1_IO03 输出低电平，打开 LED0
    GPIO1->DR &= ~(1 << 3);
}

/**
 * 打开 LED 灯
 */
void led_on(void)
{
    // 将 GPIO1_DR 的 bit3 清零
    GPIO1->DR &= ~(1<<3);
}

/**
 * 关闭 LED 灯
 */
void led_off(void)
{
    // 将 GPIO1_DR 的 bit3 置位
    GPIO1->DR |= (1<<3);
}