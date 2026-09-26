/* Umicom Framework teaching project: assembly / stock_total.S
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
/* GNU assembler, AT&T syntax. This leaf routine touches no stack storage and
 * uses only caller-saved registers for the selected ABI. */
.text
.globl StockTotal
#ifndef _WIN32
.type StockTotal, @function
#endif
StockTotal:
    xorl %eax, %eax
#ifdef _WIN32
    testq %rdx, %rdx
    je .Ldone
.Lagain:
    addl (%rcx), %eax
    addq $4, %rcx
    decq %rdx
#else
    testq %rsi, %rsi
    je .Ldone
.Lagain:
    addl (%rdi), %eax
    addq $4, %rdi
    decq %rsi
#endif
    jne .Lagain
.Ldone:
    ret
#ifndef _WIN32
.size StockTotal, .-StockTotal
.section .note.GNU-stack,"",@progbits
#endif
