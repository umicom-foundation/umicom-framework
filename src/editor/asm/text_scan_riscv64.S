/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/asm/text_scan_riscv64.S
 *
 * PURPOSE:
 *   Implement the text scan riscv64 behaviour used by its public contract and
 *   client applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework RISC-V 64 scalar byte scanner.
 * RV64 ABI: a0=bytes, a1=count, a2=value, a0=result.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/

    .text
    .align 2
    .global umi_editor_text_scan_count_byte_riscv64
    .type umi_editor_text_scan_count_byte_riscv64, @function
/* Preserve the input pointer and count matching bytes within the supplied length. */
umi_editor_text_scan_count_byte_riscv64:
    mv t0, a0
    li t1, 0
    li a0, 0
    beqz a1, .Lumi_scan_rv_done
.Lumi_scan_rv_loop:
    /* Read the indexed byte and advance the count only for an exact match. */
    add t2, t0, t1
    lbu t3, 0(t2)
    bne t3, a2, .Lumi_scan_rv_next
    addi a0, a0, 1
.Lumi_scan_rv_next:
    addi t1, t1, 1
    bltu t1, a1, .Lumi_scan_rv_loop
.Lumi_scan_rv_done:
    /* Return after all bounded input bytes have been considered. */
    ret
    .size umi_editor_text_scan_count_byte_riscv64, .-umi_editor_text_scan_count_byte_riscv64

#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",@progbits
#endif
