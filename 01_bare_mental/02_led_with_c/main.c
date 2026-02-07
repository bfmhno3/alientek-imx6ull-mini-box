#include "main.h"

/**
 * 主函数
 * @return
 */
int main(void)
{
    clk_enable();
    led_init();

    while (1)
    {
        led_off();
        delay_ms(500);

        led_on();
        delay_ms(500);
    }

    return 0;
}

/**
 * 使能 I.MX6ULL 所有外设时钟
 * @param 无
 * @return 无
 */
void clk_enable(void)
{
    CCM_CCGR0 = 0xffffffff;
    CCM_CCGR1 = 0xffffffff;
    CCM_CCGR2 = 0xffffffff;
    CCM_CCGR3 = 0xffffffff;
    CCM_CCGR4 = 0xffffffff;
    CCM_CCGR5 = 0xffffffff;
    CCM_CCGR6 = 0xffffffff;
}


/**
 * 初始化 LED 对应的 GPIO
 */
void led_init(void)
{
    // 1. 初始化 IO 复用，复用为 GPIO1_IO03
    IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 = 0x5;

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
    IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 = 0x10b0;

    // 3. 初始化 GPIO，GPIO1_IO03 设置为输出
    GPIO1_GDIR = 0x00000008;

    // 4. 设置 GPIO1_IO03 输出低电平，打开 LED0
    GPIO1_DR = 0x0;
}


/**
 * 打开 LED 灯
 */
void led_on(void)
{
    // 将 GPIO1_DR 的 bit3 清零
    GPIO1_DR &= ~(1<<3);
}

/**
 * 关闭 LED 灯
 */
void led_off(void)
{
    // 将 GPIO1_DR 的 bit3 置位
    GPIO1_DR |= (1<<3);
}

/**
 * 短时间延时函数
 * @param n 要延时循环次数（空操作循环次数，模式延时）
 * @return 无
 */
void delay_short(unsigned int n)
{
    while (n--)
    {
        ;
    }
}

/**
 * 延时函数，在 396MHz 主频下延时时间大约位 1ms
 * @param ms 要延时的毫秒数
 */
void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        delay_short(0x7ff);
    }
}
