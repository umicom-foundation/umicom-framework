/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/asm/fingerprint_x86_64.S
 *
 * PURPOSE:
 *   Implement the fingerprint x86 64 behaviour used by its public contract and
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
/* Hash each caller-owned byte with the supplied seed using the FNV-1a rule. */
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
    /* Fold one byte into the hash, then multiply by the fixed FNV prime. */
    movzbl (%r10), %ecx
    xorq %rcx, %rax
    imulq %r11, %rax
    incq %r10
    decq %r9
    jne .Lumi_document_hash_loop
.Lumi_document_hash_done:
    /* Return the final fingerprint without retaining a pointer to the document. */
    ret
#ifndef _WIN32
    .size umi_document_hash_bytes_asm, .-umi_document_hash_bytes_asm
    .section .note.GNU-stack,"",@progbits
#endif
