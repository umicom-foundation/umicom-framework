/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/asm/fingerprint_aarch64.S
 *
 * PURPOSE:
 *   Accelerate FNV-1a document fingerprints on AArch64 platforms.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
    .text
    .align 2
    .global umi_document_hash_bytes_asm
    .type umi_document_hash_bytes_asm, %function
umi_document_hash_bytes_asm:
    movz x4, #0x01b3
    movk x4, #0x0000, lsl #16
    movk x4, #0x0100, lsl #32
    cbz x1, 2f
1:
    ldrb w3, [x0], #1
    eor x2, x2, x3
    mul x2, x2, x4
    subs x1, x1, #1
    b.ne 1b
2:
    mov x0, x2
    ret
    .size umi_document_hash_bytes_asm, .-umi_document_hash_bytes_asm
    .section .note.GNU-stack,"",%progbits
