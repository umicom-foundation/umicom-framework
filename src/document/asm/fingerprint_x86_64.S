/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/asm/fingerprint_x86_64.S
 *
 * PURPOSE:
 *   Accelerate FNV-1a document fingerprints on Windows and System V x86-64.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
    .text
    .globl umi_document_hash_bytes_asm
#ifdef _WIN32
    .def umi_document_hash_bytes_asm; .scl 2; .type 32; .endef
#else
    .type umi_document_hash_bytes_asm, @function
#endif
umi_document_hash_bytes_asm:
#ifdef _WIN32
    movq %rcx, %r10
    movq %rdx, %r9
    movq %r8, %rax
#else
    movq %rdi, %r10
    movq %rsi, %r9
    movq %rdx, %rax
#endif
    movabsq $1099511628211, %r11
    testq %r9, %r9
    je .Lumi_document_hash_done
.Lumi_document_hash_loop:
    movzbl (%r10), %ecx
    xorq %rcx, %rax
    imulq %r11, %rax
    incq %r10
    decq %r9
    jne .Lumi_document_hash_loop
.Lumi_document_hash_done:
    ret
#ifndef _WIN32
    .size umi_document_hash_bytes_asm, .-umi_document_hash_bytes_asm
    .section .note.GNU-stack,"",@progbits
#endif
