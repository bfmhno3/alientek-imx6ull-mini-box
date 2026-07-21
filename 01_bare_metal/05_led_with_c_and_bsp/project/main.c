#include "main.h"

#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_clk.h"

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
