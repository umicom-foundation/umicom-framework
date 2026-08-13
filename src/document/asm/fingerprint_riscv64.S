/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/asm/fingerprint_riscv64.S
 *
 * PURPOSE:
 *   Accelerate FNV-1a document fingerprints on RISC-V 64 platforms.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
    .text
    .align 2
    .globl umi_document_hash_bytes_asm
    .type umi_document_hash_bytes_asm, @function
umi_document_hash_bytes_asm:
    li t1, 1099511628211
    beqz a1, 2f
1:
    lbu t0, 0(a0)
    xor a2, a2, t0
    mul a2, a2, t1
    addi a0, a0, 1
    addi a1, a1, -1
    bnez a1, 1b
2:
    mv a0, a2
    ret
    .size umi_document_hash_bytes_asm, .-umi_document_hash_bytes_asm
    .section .note.GNU-stack,"",@progbits
