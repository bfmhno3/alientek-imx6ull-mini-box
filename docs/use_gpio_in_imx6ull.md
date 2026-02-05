# 在 I.MX6ULL 中使用 GPIO（汇编）

控制任何微处理器的 GPIO，本质上都是 **CPU 通过总线向特定的物理内存地址写入二进制数值**，从而改变芯片内部逻辑门电路的物理状态。

对于 I.MX6ULL 这种 Cortex-A7 架构的 SoC，其 GPIO 控制是通过 **MMIO**（*Memory Mapped I/O*）实现的。也就是说，**寄存器在 CPU 看来就是内存地址**。

使用汇编控制 GPIO 的过程，可以拆解为以下五个步骤：

## 1. 使能时钟

CMOS 电路只有在时钟信号驱动下才能翻转状态。为了**降低功耗**，SoC 默认关闭了绝大多数外设模块（包括 GPIO）的时钟信号。如果时钟是断开的，写入寄存器的操作将无效（就像对着断电的电视按遥控器）。

I.MX6ULL 中 GPIO 对应的时钟控制器为 `CCM_CCGRx`（*Clock Controller Moduke - Clock Gating Register*），其中 `x` 的取值范围为 $[0,6]$ 共 7 个寄存器。

具体操作步骤为：

1. **查阅手册**，找到控制你所使用的 GPIO 端口属于哪个 `CCGR` 寄存器的哪两位。
2. 将这两位设置为 `11`。

写入值的含义：

| Value | Clock Activity Description                                   | 解释                                                         |
| ----- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| `00`  | Clock is off during all modes.<br />**STOP** enter hardware handshake is disable. | 1. 时钟在所有的模式下都关闭，对应的外设无法工作。<br />2. 禁用进入 STOP 模式所需的硬件握手机制，从而加快系统进入低功耗状态的速度。 |
| `01`  | Clock is on in **RUN** mode, but off in **WAIT** and **STOP** modes. | 1. 只在 RUN 模式下时钟才开启。<br />2. 当系统进入 WAIT 或 STOP 状态时，为了省点，时钟会自动关掉。 |
| `10`  | Not applicable (**Reserved**).                               | 不要使用这个值，可能会导致未定义的行为。                     |
| `11`  | Clock is on during all modes, except **STOP** mode.          | 1. 除了 STOP 模式，时钟一直开启。<br />2. **最常用的配置**，确保外设在 RUN 和 WAIT 模式下都能正常工作。 |

> [!WARNING]
>
> **Module should be stopped, before set its bits to '0'; clock to the module will be stopped immediately.**
> 意思是说，在软件里关闭时钟前（比如在汇编代码中向 CCM_CCGRx 寄存器中写入 `00`），**先确保你已经关闭了该外设的功能**（比如禁用串口发送），否则直接切断时钟可能导致总线挂死。

## 2. 引脚复用配置

芯片的物理引脚（Pad）数量是有限的，但内部的控制器（I2C, UART, GPIO, SDIO 等）非常多。为了**节省引脚**，SoC 使用了 **IOMUX**（*Input/Output Multiplexing*，输入输出多路复用器）。所以需要手动将内部的 GPIO 控制器与外部的物理引脚 “连通”（本质上依旧是通过写入数值来改变电路状态）。

涉及到的寄存器为：`IOMUXC_SW_MUX_CTL_PAD_<PIN_NAME>`。与 IO 有关的寄存器命名为：`IOMUXC_SW_MUX_CTL_PAD_GPIOx_IOxx`，其中 Bit 0 $\sim$ Bit 3（共 4 位）为 MUX_MODE（*Multiplexer Mode*，复用模式选择），Bit 4 为 SION（*Software Input On*，软件强制输入开启）。

MUX_MODE 共 $2^4=8$ 种模式，其中**第 5 种模式（ALT5：`0b0101=0x5`）统一为开启 GPIO 功能**。

具体操作步骤为：

1. **查阅手册**，找到目标引脚对应的 MUX 寄存器地址。
2. 写入 `0x5`。

## 3. 电气属性配置

信号不仅仅是 0 和 1 的**逻辑概念**，更是**物理**电压和电流。所以需要定义这个引脚的物理特性，如驱动能力（Drive Strength）、压摆率（Slew Rate）、上下拉电阻（Pull Up/Down） 等，以**匹配外部电路的阻抗和电气要求**。

涉及到的寄存器为 `IOMUXC_SW_PAD_CTL_PAD_<PIN_NAME>`。与 IO 有关的寄存器命名为：`IOMUXC_SW_PAD_CTL_PAD_GPIOx_IOxx`。

`IOMUXC_SW_PAD_CTL_PAD_GPIOx_IOxx` 的位域定义如下：

| Bit  | [31, 17] | 16   | [15, 14] | 13   | 12   | 11   | [10, 8]  | [7, 6] | [5, 4] | [2, 1]   | 0    |
| ---- | -------- | ---- | -------- | ---- | ---- | ---- | -------- | ------ | ------ | -------- | ---- |
| 定义 | Reserved | HYS  | PUS      | PUE  | PKE  | ODE  | Reserved | SPEED  | DSE    | Reserved | SRE  |

对位域的解释说明：

| 位域  | 解释                             | 备注                                                 |
| ----- | -------------------------------- | ---------------------------------------------------- |
| HYS   | Hysteresis，施密特触发器（迟滞） | 读按键/抗干扰时打开                                  |
| PUS   | Pull up/Down Config，电阻值选择  | 选 47K/100K 上拉，或者 100K 下拉                     |
| PUE   | Pull/Keep Select，模式选择       | 一般选 Pull（上下拉），除非你知道为什么是要用 Keeper |
| PKE   | Pull/Keep Enable，总开关         | 设为 `1` 才能让 PUS/PUE 生效                         |
| ODE   | Open Drain，开漏输出             | 只有 I2C 协议设为 `1`，其他都是 `0`                  |
| SPEED | Speed，带宽/频率                 | 点灯选 50MHz，屏幕/网口选 100/200MHz                 |
| DSE   | Drive Strength，驱动能力         | 点灯/长线选 R0/7（最大），其他选 R0/4                |
| SRE   | Slew Rate，压摆率                | 默认选 Slow（防干扰），高速通信选 Fast               |

具体操作步骤：

- 通常直接向这个寄存器写入一个 16 进制数，该数值的每一位对应不同的电气属性。

## 4. 配置 GPIO 方向

GPIO 内部有一个三态缓冲器（*Tri-state Buffer*），需要告诉控制，是要**驱动**这个线路（输出电压），还是想要**侦听**这个线路（读取电压）。

涉及到的寄存器为 `GPIOx_GDIR`（GPIO Direction Register）。

具体操作步骤为：

- `0` 代表输入（Input）
- `1` 代表输出（Output）
- 通过位操作，将对应引脚的位设置为 `1` 或 `0`。

## 5. 设置输出电平

通过改变寄存器中触发器的状态，驱动物理引脚输出高电平（VCC）或低电平（GND）。

涉及到的寄存器为 `GPIOx_DR`（GPIO Data Register）。

具体操作为：

- 写 `0` 输出低电平
- 写 `1` 输出高电平

