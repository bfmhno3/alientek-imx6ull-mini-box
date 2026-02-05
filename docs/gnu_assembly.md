# GNU Assembly (GAS) 核心指南

由于机器指令及助记符高度依赖于芯片的具体架构，本文不讨论特定指令集的全部细节，而是聚焦于 GAS（GNU Assembler）通用的伪指令系统及其基本语法规则。

## 汇编器的本质与工作流

GNU Assembler（简称 `as`）的核心工作流可以用以下公式概括：

$$
\text{源代码 (.s)} \xrightarrow{\text{汇编器 (as)}} \text{可重定位目标文件 (.o)}
$$

生成的 `.o` 文件主要由 **Section**（段）和 **Symbol Table**（符号表）组成。我们常用的 GAS 伪指令（Directives/Pseudo-ops），其根本目的就是为了控制这两个核心要素的生成。

从本质上来说，GNU Assembler 的核心职能并非简单的 “代码翻译”，而是**内存空间的规划**与**符号地址的解析**。

汇编器内部维护着一个至关重要的变量：**地址计数器**（Location Counter，通常记为 `.`）。当汇编器读取源代码时，它实际上是在执行一个状态机循环：

1. **解析伪指令**：根据指令调整地址计数器的位置（例如跳转到新的段、预留空白空间）或修改符号的属性（如设置为全局可见）。
2. **发射字节流**：将翻译好的机器指令或定义的静态数据 “填充” 到地址计数器当前指向的内存单元，随后让计数器自增，指向下一个空闲位置。
3. **构建符号表**：记录所有标号（Label）对应的最终物理地址或相对于段的偏移量。

因此，当我们编写汇编代码时，实际上是在做两件事：

1. **填充内存**：明确告诉汇编器，在当前的内存地址上应当放置什么数据（注意：机器指令在内存中本质上也是数据）。
2. **标记位置**：给特定的内存地址起一个人类可读的名字（Label/Symbol），以便后续进行跳转或读写操作。

## 指令语法

GAS 语法的核心难点在于不同架构下对**操作数方向**的处理，以及如何消除符号的**歧义**。

### 注释风格

不同的 CPU 架构在 GAS 中通常使用不同的单行注释符号。这里仅列举常见架构的规范，更详细的列表可查阅 [Binutils Docs: Machine Dependent Features](https://sourceware.org/binutils/docs/as/Machine-Dependencies.html)。

> [!NOTE]
>
> 推荐直接查阅 "[Machine Dependent Features](https://sourceware.org/binutils/docs/as/Machine-Dependencies.html)" 文档，官方通用的 "[Comments](https://sourceware.org/binutils/docs/as/Comments.html)" 文档包含大量历史遗留信息，较为混乱。

**多行注释**在所有架构中是通用的，沿用了 C 语言风格：

```assembly
/*
 The only way to include a newline ('\n') in a comment
 is to use this sort of comment.
 */

/* This sort of comment does not nest. */
```

**单行注释**则因架构而异：

#### x86-64

根据 [Binutils Docs: i386_002dChars](https://sourceware.org/binutils/docs/as/i386_002dChars.html) 的说明：

> The presence of a `#` appearing anywhere on a line indicates the start of a comment that extends to the end of that line.
>
> If a `#` appears as the first character of a line then the whole line is treated as a comment, but in this case the line can also be a logical line number directive...
>
> If the --divide command line option has not been specified then the `/' character appearing anywhere on a line also introduces a line comment.
>
> The `;` character can be used to separate statements on the same line.

**核心解读：**

- **首选注释符**：使用 `#`。只要行内出现 `#`，其后内容均为注释。
- **特殊情况**：如果 `#` 出现在行首，汇编器会优先检查它是否为预处理命令（如 `#include`）或逻辑行号。
- **避坑指南**：虽然默认情况下 `/` 也可以作为注释开始，但这极易产生歧义。例如在指令 `movq $(10/2), %rax` 中，汇编器可能会把 `/2` 误判为注释。除非指定了 `--divide` 选项，否则**严禁使用 `/` 写注释**。
- **语句分隔**：分号 `;` 用于在同一行写多条指令，类似 C 语言。

#### ARM (32-bit)

根据 [Binutils Docs: ARM_002dChars](https://sourceware.org/binutils/docs/as/ARM_002dChars.html) 的说明：

> The presence of a `@` anywhere on a line indicates the start of a comment that extends to the end of that line.
>
> If a `#` appears as the first character of a line then the whole line is treated as a comment...
>
> The `;` character can be used instead of a newline to separate statements.
>
> Either `#` or `$` can be used to indicate immediate operands.

**核心解读：**

- **首选注释符**：使用 `@`。这是最安全的方式。
- **分号的作用**：在 GNU ARM 汇编中，`;` 是语句分隔符（注意：这与 ARM 官方汇编器如 Keil/armasm 不同，后者用 `;` 做注释）。
- **`#` 的双重身份**：
  - 如果在**行首**，它是注释。
  - 如果在**指令中**，它表示立即数（例如 `MOV R0, #10`）。
  - **警告**：切勿在指令行尾使用 `#` 做注释，汇编器会将其误认为是非法的立即数格式。
- **立即数**：虽然支持 x86 风格的 `$`，但为了代码移植性，**强烈建议使用 `#` 表示立即数**。

#### AArch64 (ARM64)

GNU 对 32 位 ARM 和 64 位 AArch64 做了严格区分，语法有所不同。

[Binutils Docs: AArch64_002dChars](https://sourceware.org/binutils/docs/as/AArch64_002dChars.html) 说明如下：

> The presence of a `//` on a line indicates the start of a comment that extends to the end of the current line.
>
> If a `#` appears as the first character of a line, the whole line is treated as a comment.
>
> The `#` can be optionally used to indicate immediate operands.

**核心解读：**

- **首选注释符**：使用 `//`（C++ 风格）。
- **立即数**：`#` 是可选的，但在阅读习惯上保留它更为清晰。

#### RISC-V

RISC-V 的 GAS 实现较为现代，通常兼容性较好：

- 支持行首 `#` 注释。
- 通常支持 `//` 或 `#` 作为行尾注释（具体取决于工具链版本，建议测试确认）。

------

### 最大的分歧：数据流向（Direction）

阅读 GAS 代码的第一道门槛是**源操作数与目的操作数的顺序**。GAS 历史上继承了 **AT&T** 语法（源自 Unix），而芯片厂商文档通常使用 **Intel/Native** 语法。

#### x86 环境下的 GAS（AT&T 风格）

x86 GAS 遵循 “**源在前，目的在后**” 的原则。读代码时请默念“把左边的值给右边”。

```
operation source, destination
```

例如：

```
movl %eax, %ebx  # 含义：EBX = EAX (将 EAX 的值搬运到 EBX)
```

#### ARM/RISC-V 环境下的 GAS（官方风格）

为了减少混乱，GNU 针对 ARM 和 RISC-V 做了妥协，采用了更符合官方文档的 “**目的在前，源在后**” 顺序：

```assembly
operation destination, source
```

例如：

```assembly
mov r0, r1       # 含义：r0 = r1 (将 r1 的值搬运到 r0)
```

### 消除歧义：前缀与修饰符

汇编器必须通过“前缀”来区分一个数字到底是**数值**、**地址**还是**寄存器**。

#### 寄存器

- **x86 (AT&T)**：必须加 `%`，例如 `%eax`, `%esp`。
- **ARM/RISC-V**：通常不加前缀，直接写 `r0`, `sp`, `pc`。为了避免与 C 语言变量名冲突，偶尔可见 `%r0` 写法，但不常见。

#### 立即数

汇编器如何知道 `10` 是指“内存地址 10”还是“数字 10”？

- **x86 (AT&T)**：数字前必须加 `$`。
- **ARM**：官方推荐加 `#`。
- **RISC-V**：极其精简，通常不加前缀，依靠指令名（如 `li`, `addi`）的上下文自动推断。

**对比示例：**

```assembly
# x86 (AT&T)
movl $10, %eax   # EAX = 10 (立即数)
movl 10, %eax    # EAX = *(*int)(0x0A) (读取内存地址 10 的内容)

# ARM
MOV R0, #10      @ R0 = 10

# RISC-V
li a0, 10        @ Load Immediate: a0 = 10
```

### 内存寻址模式

在 64 位系统中，内存地址空间高达 $2^{64}$，需要 8 字节才能完整表达。然而，一条机器指令通常只有 4 字节（RISC）或变长（x86）。如何在有限的指令长度内精确指向遥远的内存地址？这是寻址模式设计的核心挑战。

#### x86 (AT&T)

x86 是 CISC（复杂指令集），其最大特点是**允许运算指令直接操作内存**。硬件内建了强大的 SIB（Scale-Index-Base）寻址单元，将地址计算视为一个多项式：

**通用格式**：

```assembly
offset(base, index, scale)
```

**计算公式**：

$$
\text{Address} = \text{base} + (\text{index} \times \text{scale}) + \text{offset}
$$

**应用场景**：这非常适合访问数组。例如 `array[i]`，其中 `base` 是数组首地址，`index` 是变量 `i`，`scale` 是元素大小（如 `int` 为 4）。

```assembly
movl 4(%ebp), %eax    # 取地址 (ebp + 4) 处的数据 -> eax
movl array(,%edi,4), %eax # 取地址 (array + edi * 4) 处的数据 -> eax
```

#### ARM

ARM 是 RISC 且为 **Load/Store 架构**。这意味着不能直接对内存里的数进行 `add` 等运算，必须先 `ldr`（Load）到寄存器，运算后再 `str`（Store）回去。

ARM 的特色在于 **Address Write-back（地址回写）**。为了方便遍历数组，它允许在读写内存的同时，自动更新基址寄存器的值。

GAS 中使用中括号 `[]` 表示内存引用：

- **常规偏移**：`[Base, Offset]`
- **寄存器偏移**：`[Base, Register]`，甚至支持移位 `[Base, Reg, lsl #3]`。
- **前索引（Pre-indexed）**：`[x1, #8]!` —— 访问前先加 8，且**修改** x1 的值（x1 += 8）。
- **后索引（Post-indexed）**：`[x1], #8` —— 访问后再加 8，且**修改** x1 的值。

#### RISC-V

RISC-V 追求极致的硬件精简。它不支持 ARM 那种复杂的自动回写，也不支持 x86 的复杂比例因子。

RISC-V 的指令长度严格固定为 32 位，除去操作码和寄存器位，留给立即数偏移的空间只有 **12 位**（即 $\pm 2\text{KB}$）。

**通用格式**：

```assembly
offset(base)
```

**计算公式**：

$$
\text{Address} = \text{base} + \text{offset}
$$

**如何访问大范围地址？**

由于 12 位无法容纳 32/64 位地址，必须将大地址拆分为“高位”和“低位”两条指令处理：

```assembly
# 小范围寻址 (Offset < 2048)
lw a0, 8(sp)        # 从 sp + 8 处加载一个 word
sw a0, 0(t1)        # 存入 t1 + 0 处

# 大范围/全局变量寻址 (Hi/Lo 拆分)
lui t0, %hi(VAL)      # 加载 VAL 地址的高 20 位到 t0
lw  a0, %lo(VAL)(t0)  # 加载 VAL 地址的低 12 位，作为偏移加到 t0 上读取
```

### 指令后缀：操作数宽度 (Size Suffixes)

在机器层面，CPU 处理数据时必须明确一个核心参数：**位宽 (Width)**。硬件需要知道它是应该只驱动数据总线上的 8 根线，还是激活全部 64 根线？是将数据放入寄存器的低 8 位（如 `AL`），还是覆盖整个 64 位（如 `RAX`）？

GAS 通过在指令助记符后添加**后缀**来显式控制这一行为。

#### GAS 通用命名规则

GAS 使用一套单字母后缀来表示数据宽度。需要特别警惕的是，不同架构对 “Word” 的定义存在历史分歧。

| **后缀** | **全称** | **大小 (Bits)** | **字节数** | **说明**                                                     |
| -------- | -------- | --------------- | ---------- | ------------------------------------------------------------ |
| **b**    | **B**yte | 8-bit           | 1          | 通用。                                                       |
| **w**    | **W**ord | **16-bit**      | 2          | **注意！** 在 x86 语境下，Word 是 16 位（源于 8086）。但在 ARM/RISC-V 文档中，Word 通常指 32 位。GAS 的 `w` 后缀专指 16 位。 |
| **l**    | **L**ong | 32-bit          | 4          | x86 的标准 32 位后缀。                                       |
| **q**    | **Q**uad | 64-bit          | 8          | 四字（4 x 16-bit），即 64 位。                               |

#### x86 架构：显式后缀 (Explicit Suffixes)

x86 是变长指令集，且寄存器存在嵌套关系（`RAX` 包含 `EAX` 包含 `AX` 包含 `AL`）。为了消除歧义，GAS **强烈推荐**在所有操作码后加上后缀。

虽然现代汇编器有时能根据寄存器名字（如 `%eax`）推断出你是想操作 32 位，但在涉及内存操作数（无法看出大小）或立即数时，不写后缀极易报错。

```assembly
movb $0xFF, %al      	# 移动 1 个字节 (8-bit)
movw $0xFFFF, %ax    	# 移动 1 个字   (16-bit)
movl $0xFFFFFFFF, %eax	# 移动 1 个长字 (32-bit)
movq $1, %rax        	# 移动 1 个四字 (64-bit)

# 歧义示例：
# mov $0, (%rbx)     # 错误！汇编器不知道你想把 rb 指向的内存清零 1 个字节还是 8 个字节。
movq $0, (%rbx)      # 正确：清零 8 个字节
```

#### ARM / RISC-V 架构：助记符内含宽度

与 x86 不同，ARM 和 RISC-V 等 RISC 架构通常将数据宽度直接**编码在指令助记符本身**中，而不是依赖 GAS 的通用后缀。

在这些架构中，你通常不会看到 `movl` 这种写法，而是通过不同的指令变体来实现：

- **RISC-V**:
  - `lb` (Load Byte): 加载 8 位
  - `lh` (Load Half-word): 加载 16 位
  - `lw` (Load Word): 加载 32 位 (**注意：这里 Word 是 32 位**)
  - `ld` (Load Double-word): 加载 64 位
- **ARM**:
  - `LDRB` (Byte): 加载 8 位
  - `LDRH` (Half-word): 加载 16 位
  - `LDR` (Word): 加载 32 位

> [!CAUTION]
>
> **"Word" 的陷阱**
>
> 这是一个跨架构开发时最容易混淆的概念：
>
> - 在 **x86** 汇编中，`Word` = **16 bits**。
> - 在 **ARM / RISC-V** 汇编中，`Word` = **32 bits**。
>
> 当你使用 GAS 的 `.word` 伪指令定义数据时，它在所有架构上通常都表示 32 位（或 16 位，取决于具体配置，但在现代环境下多为 32 位）。务必查阅具体目标架构的文档确认 `.word` 的确切长度，或者直接使用 `.2byte` / `.4byte` 这种无歧义的写法。

## 伪指令系统 (Directives)

伪指令以 `.` 开头，它们不对应具体的 CPU 机器码，而是指挥汇编器如何组织二进制结构。

### 数据定义：如何填充字节

这些指令直接在当前地址计数器位置“发射”数据，常用于定义查找表或初始化变量。

- `.byte` / `.hword` / `.word` / `.long` / `.quad`：

  在当前地址写入特定宽度的整数。注意**字节序（Endianness）**取决于目标架构（x86 为小端，ARM/RISC-V 默认为小端，但可配置）。

  代码段

  ```assembly
  .byte 0x12, 0x34    # 写入两个字节
  .word 0xAABBCCDD    # 写入 4 字节。小端模式下内存为：DD CC BB AA
  ```

- `.ascii` vs `.asciz` / `.string`：

  - `.ascii "Hello"`：仅填入字符 ASCII 码，**无**结尾 `\0`。
  - `.asciz "Hello"` 或 `.string`：自动在末尾补 `0x00`（C 语言标准字符串）。

- `.fill` / `.skip` / `.space` / `.zero`：

  用于预留空间或清零，常用于定义栈（Stack）或 BSS 段。

  代码段

  ```assembly
  stack_top:
  .skip 1024, 0      # 预留 1024 字节，全部填 0
  stack_bottom:      # 栈底标号（假设栈向下生长，SP 初始指向此处）
  ```

### 内存控制：如何管理地址

- `.section`：

  告诉汇编器：“**接下来的数据/指令，请分类归档到哪个‘抽屉’里**”。

  - `.section .text`：代码段（只读，可执行）。
  - `.section .data`：已初始化的全局变量（可读写）。
  - `.section .bss`：未初始化变量（不占磁盘空间，程序启动时自动清零）。
  - **自定义段**：例如 `.section .isr_vector`，配合链接脚本（Linker Script），可以将中断向量表精确固定在 Flash 的 0 地址处。

- `.align` vs `.balign`：

  - `.align`：行为不统一！在 x86 下 `.align 4` 表示 4 字节对齐；但在 ARM 下可能表示 $2^4=16$ 字节对齐。
  - `.balign`（Byte Align）：**推荐使用**。`.balign 4, 0` 明确表示保证地址能被 4 整除，如果需要填充，则填 0。

- `.org`：

  强制修改当前的地址计数器。

  - **示例**：编写 MBR 引导扇区时，需要在第 510 字节写结束标志 `0x55AA`，会用到 `.org 510`。

### 符号管理：与 C 语言的交互接口

这是 C 和汇编混合编程的桥梁。

- `.global`（或 `.globl`）：

  将符号标记为**全局可见**。如果不加此标记，符号仅在当前文件内部可见（类似 C 的 `static`），链接器无法找到它。

  - **场景**：Startup 文件（启动文件）必须声明 `.global Reset_Handler`，否则硬件复位后找不到入口。

- `.extern`：

  声明符号在外部定义。虽然 GAS 通常能自动推断，但显式声明能提高代码可读性。

- `.equ` / `.set`：

  常量定义，相当于 C 的 `#define`。它们只存在于汇编阶段，不占用运行时的内存空间。

  代码段

  ```assembly
  .equ STACK_SIZE, 0x400
  ```

- `.type`：

  明确告诉链接器符号的类型（是函数还是数据）。

  - **重要性**：在 ARM Thumb-2 指令集中，如果链接器知道某符号是 `%function`，它会在计算跳转地址时自动将最低位置 1（Thumb 状态标志）。若省略此指令，可能导致切换状态失败而进入 HardFault。

- `.weak`：

  **弱符号定义**。

  - **机制**：如果在其他地方定义了同名的强符号（普通的 `.global`），链接器优先使用那个；如果没有，则使用这里的默认定义。
  - **应用**：STM32 启动文件中的中断处理函数通常用 `.weak` 定义为死循环。这允许用户在 C 代码中重写同名函数（如 `SysTick_Handler`）来覆盖默认行为，而无需修改启动代码。

## GAS 的特色功能 (Syntactic Sugar)

### 局部数值标签 (Local Numeric Labels)

为了避免给简单的循环绞尽脑汁起名字（如 `loop1`, `loop_start`），GAS 允许使用数字作为临时标号。

- **定义**：`<num>:`
- **引用**：`<num>f` (forward, 向前/向下找)，`<num>b` (backward, 向后/向上找)。

**示例**：

代码段

```assembly
1:          # 定义标号 1
    wfi     # Wait for interrupt
    j 1b    # 向后跳转到最近的标号 1 (相当于 while(1);)
```

### 宏 (Macros)

用于封装重复代码片段：

代码段

```assembly
.macro 宏名称 参数1, 参数2
    // 宏体内容，可使用 \参数1 引用
.endm
```

### `.` (Current Location)

符号 `.` 代表当前地址计数器的值。

- **常见用法**：`b .` 等同于 `while (1);`（跳转到当前地址，死循环）。