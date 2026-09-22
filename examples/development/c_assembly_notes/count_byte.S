/*-----------------------------------------------------------------------------
 * Umicom Framework — C and Assembly example
 * File: examples/development/c_assembly_notes/count_byte.S
 * PURPOSE: Count one byte using the x86-64 C ABI without touching the stack.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
.text
.globl UmiNotesCountByte
#ifndef _WIN32
.type UmiNotesCountByte, @function
#endif
UmiNotesCountByte:
#ifdef _WIN32
    /* Microsoft x64: RCX=text, RDX=length, R8B=value. */
    movq %rcx, %r9
    movq %rdx, %rcx
#else
    /* System V AMD64: RDI=text, RSI=length, DL=value. */
    movq %rdi, %r9
    movq %rsi, %rcx
    movb %dl, %r8b
#endif
    xorl %eax, %eax
    testq %rcx, %rcx
    jz .Lnotes_done
.Lnotes_next:
    cmpb %r8b, (%r9)
    jne .Lnotes_advance
    incq %rax
.Lnotes_advance:
    incq %r9
    decq %rcx
    jnz .Lnotes_next
.Lnotes_done:
    ret
#ifndef _WIN32
.size UmiNotesCountByte, .-UmiNotesCountByte
.section .note.GNU-stack,"",@progbits
#endif
