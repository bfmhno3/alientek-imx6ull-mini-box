# 在 STM32 中使用 GPIO

一般流程为：

1. 使能 GPIO 时钟，例如：`__HAL_RCC_GPIOA_CLK_ENABKE()`
2. 定义并填充初始化结构体 `GPIO_InitTypeDef`，主要配置以下内容：引脚、模式（推挽、开漏、复用推挽、输入等）、上/下拉电阻、引脚翻转频率
3. （可选）设置引脚初始电平，例如：`HAL_GPIO_Init(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)`
4. 调用初始化函数，例如：`HAL_GPIO_Init(GPIOA, &gpio_init_struct)`
5. 使用 GPIO（读/写/翻转）