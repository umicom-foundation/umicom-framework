/* Audited RISC-V 64 scalar byte counter for terminal transcript scans. */
    .text
    .align 2
    .globl umi_terminal_scan_count_byte_riscv64
    .type umi_terminal_scan_count_byte_riscv64, @function
umi_terminal_scan_count_byte_riscv64:
    li a3, 0
    beqz a1, 2f
1:
    lbu a4, 0(a0)
    addi a0, a0, 1
    bne a4, a2, 3f
    addi a3, a3, 1
3:
    addi a1, a1, -1
    bnez a1, 1b
2:
    mv a0, a3
    ret
    .size umi_terminal_scan_count_byte_riscv64, .-umi_terminal_scan_count_byte_riscv64
#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",@progbits
#endif
