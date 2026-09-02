/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/asm/text_scan_aarch64.S
 *
 * PURPOSE:
 *   Implement the text scan aarch64 behaviour used by its public contract and
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
 * Umicom Framework AArch64 scalar byte scanner.
 * AAPCS64: x0=bytes, x1=count, w2=value, x0=result.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/

    .text
    .align 2
    .global umi_editor_text_scan_count_byte_aarch64
    .type umi_editor_text_scan_count_byte_aarch64, %function
/* Preserve the source pointer and count matches using the AArch64 calling convention. */
umi_editor_text_scan_count_byte_aarch64:
    mov x3, x0
    mov x4, #0
    mov x0, #0
    cbz x1, .Lumi_scan_arm_done
.Lumi_scan_arm_loop:
    /* Load one indexed byte and conditionally increment the returned count. */
    ldrb w5, [x3, x4]
    cmp w5, w2
    cinc x0, x0, eq
    add x4, x4, #1
    cmp x4, x1
    b.lo .Lumi_scan_arm_loop
.Lumi_scan_arm_done:
    /* Return after the loop reaches the exact caller-provided length. */
    ret
    .size umi_editor_text_scan_count_byte_aarch64, .-umi_editor_text_scan_count_byte_aarch64

#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",%progbits
#endif
