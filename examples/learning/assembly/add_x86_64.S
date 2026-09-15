/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/assembly/add_x86_64.S
 *
 * PURPOSE:
 *   Implement a leaf function for the Windows and ELF x86-64 ABIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#if !defined(__x86_64__)
#error "This file requires an x86-64 compiler target."
#endif
#if defined(__APPLE__)
#error "This lesson file supports Windows COFF and ELF, not Mach-O."
#endif
    .text
    .globl AddTwoAssembly
#if defined(__ELF__)
    .type AddTwoAssembly, @function
#endif
AddTwoAssembly:
#if defined(_WIN32)
    /* Windows: the first two 32-bit integer arguments are in ECX and EDX. */
    movl %ecx, %eax
    addl %edx, %eax
#else
    /* System V x86-64: the first two arguments are in EDI and ESI. */
    movl %edi, %eax
    addl %esi, %eax
#endif
    /* EAX contains the unsigned 32-bit result. No stack storage is needed. */
    ret
#if defined(__ELF__)
    .size AddTwoAssembly, .-AddTwoAssembly
    .section .note.GNU-stack,"",@progbits
#endif
