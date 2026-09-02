/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/asm/scan_riscv64.S
 *
 * PURPOSE:
 *   Implement the scan riscv64 behaviour used by its public contract and client
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Audited RISC-V 64 scalar byte counter for terminal transcript scans. */
    .text
    .align 2
    .globl umi_terminal_scan_count_byte_riscv64
    .type umi_terminal_scan_count_byte_riscv64, @function
/* Count matching bytes using only caller-provided buffer bounds. */
umi_terminal_scan_count_byte_riscv64:
    li a3, 0
    beqz a1, 2f
1:
    /* Load one byte, advance the buffer and count it when it matches. */
    lbu a4, 0(a0)
    addi a0, a0, 1
    bne a4, a2, 3f
    addi a3, a3, 1
3:
    addi a1, a1, -1
    bnez a1, 1b
2:
    /* Move the finished count into the ABI result register before returning. */
    mv a0, a3
    ret
    .size umi_terminal_scan_count_byte_riscv64, .-umi_terminal_scan_count_byte_riscv64
#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",@progbits
#endif
