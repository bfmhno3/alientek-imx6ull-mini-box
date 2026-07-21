#include "bsp_delay.h"

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
