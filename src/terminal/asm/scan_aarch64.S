/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/asm/scan_aarch64.S
 *
 * PURPOSE:
 *   Implement the scan aarch64 behaviour used by its public contract and client
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Audited AArch64 scalar byte counter for terminal transcript scans. */
    .text
    .align 2
    .global umi_terminal_scan_count_byte_aarch64
    .type umi_terminal_scan_count_byte_aarch64, %function
/* Count matching bytes without reading beyond the supplied AArch64 length. */
umi_terminal_scan_count_byte_aarch64:
    mov x3, #0
    cbz x1, 2f
1:
    /* Load one byte, conditionally increment the count and reduce remaining work. */
    ldrb w4, [x0], #1
    cmp w4, w2
    cinc x3, x3, eq
    subs x1, x1, #1
    b.ne 1b
2:
    /* Return the accumulated count through the standard result register. */
    mov x0, x3
    ret
    .size umi_terminal_scan_count_byte_aarch64, .-umi_terminal_scan_count_byte_aarch64
#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",@progbits
#endif
