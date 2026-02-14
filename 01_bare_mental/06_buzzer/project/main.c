#include "main.h"

#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_clk.h"
#include "bsp_buzzer.h"

/**
 * 主函数
 * @return
 */
int main(void)
{
    clk_enable();
    led_init();
    buzzer_init();

    while (1)
    {
        led_off();
        buzzer_switch(0);
        delay_ms(500);

        led_on();
        buzzer_switch(1);
        delay_ms(500);
    }

    return 0;
}
