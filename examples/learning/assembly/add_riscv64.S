/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/assembly/add_riscv64.S
 *
 * PURPOSE:
 *   Implement the same leaf function for the RISC-V 64-bit ELF ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#if !defined(__riscv) || __riscv_xlen != 64
#error "This file requires a 64-bit RISC-V compiler target."
#endif
    .text
    .globl AddTwoAssembly
    .type AddTwoAssembly, @function
AddTwoAssembly:
    /* a0 and a1 contain the arguments. addw computes the low 32-bit sum
     * and sign-extends the register result as required by the RV64 ABI. */
    addw a0, a0, a1
    ret
    .size AddTwoAssembly, .-AddTwoAssembly
    .section .note.GNU-stack,"",@progbits
