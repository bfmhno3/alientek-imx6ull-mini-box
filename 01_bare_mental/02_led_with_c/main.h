#ifndef MAIN_H__
#define MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CCM 相关寄存器地址
 */
#define CCM_CCGR0   (*(volatile unsigned int *)0x020C4068)
#define CCM_CCGR1   (*(volatile unsigned int *)0x020C406c)
#define CCM_CCGR2   (*(volatile unsigned int *)0x020C4070)
#define CCM_CCGR3   (*(volatile unsigned int *)0x020C4074)
#define CCM_CCGR4   (*(volatile unsigned int *)0x020C4078)
#define CCM_CCGR5   (*(volatile unsigned int *)0x020C407c)
#define CCM_CCGR6   (*(volatile unsigned int *)0x020C4080)

/**
 * IOMUX 相关寄存器地址
 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03  (*(volatile unsigned int *)0x020e0068)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03  (*(volatile unsigned int *)0x020e02f4)


/**
 * GPIO 相关寄存器地址
 */
#define GPIO1_DR            (*(volatile unsigned int *)0x0209c000)
#define GPIO1_GDIR          (*(volatile unsigned int *)0x0209c004)
#define GPIO1_PSR           (*(volatile unsigned int *)0x0209c008)
#define GPIO1_ICR1          (*(volatile unsigned int *)0x0209c00c)
#define GPIO1_ICR2          (*(volatile unsigned int *)0x0209c010)
#define GPIO1_IMR           (*(volatile unsigned int *)0x0209c014)
#define GPIO1_ISR           (*(volatile unsigned int *)0x0209c018)
#define GPIO1_EDGE_SEL      (*(volatile unsigned int *)0x0209c01c)

void clk_enable(void);
void led_init(void);
void led_on(void);
void led_off(void);
void delay_short(unsigned int n);
void delay_ms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif // MAIN_H__
