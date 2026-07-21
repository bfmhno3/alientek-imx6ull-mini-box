.global _start

_start:
    mrs r0, cpsr
    bic r0, r0, #0x1f @ 将 r0 的低 5 位清零，也就是 cpsr 的 M0~M4
    orr r0, r0, #0x13 @ 将 r0 的低 5 位设置为 0x13，也就是进入 SVC 模式
    msr cpsr, r0 @ 将修改后的值写回 cpsr 寄存器

    ldr sp, =0x80200000 @ 设置栈指针
    b main @ 跳转到 main 函数
