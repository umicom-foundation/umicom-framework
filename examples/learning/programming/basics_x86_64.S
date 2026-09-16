/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/basics_x86_64.S
 *
 * PURPOSE:
 *   Teach unsigned comparison and a zero-safe loop on Windows/ELF x86-64.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* GCC/Clang preprocessed Assembly, AT&T syntax: source operand first. */
#ifdef _WIN32
#define FIRST %rcx
#define SECOND %rdx
#else
#define FIRST %rdi
#define SECOND %rsi
#endif
.text
.globl LabMaximumAsm
#ifndef _WIN32
.type LabMaximumAsm, @function
#endif
LabMaximumAsm:
    movq FIRST, %rax
    cmpq SECOND, %rax
    jae .Lmaximum_done
    movq SECOND, %rax
.Lmaximum_done:
    ret
#ifndef _WIN32
.size LabMaximumAsm, .-LabMaximumAsm
#endif

.globl LabSumAsm
#ifndef _WIN32
.type LabSumAsm, @function
#endif
LabSumAsm:
    xorq %rax, %rax
    testq SECOND, SECOND
    je .Lsum_done
.Lsum_next:
    addq (FIRST), %rax
    addq $8, FIRST
    subq $1, SECOND
    jne .Lsum_next
.Lsum_done:
    ret
#ifndef _WIN32
.size LabSumAsm, .-LabSumAsm
.section .note.GNU-stack,"",@progbits
#endif
