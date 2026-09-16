/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/basics_riscv64.S
 *
 * PURPOSE:
 *   Teach leaf functions with the RISC-V LP64 integer calling convention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

.text
.globl LabMaximumAsm
.type LabMaximumAsm, @function
LabMaximumAsm:
    bgeu a0, a1, .Lmaximum_done
    mv a0, a1
.Lmaximum_done:
    ret
.size LabMaximumAsm, .-LabMaximumAsm

.globl LabSumAsm
.type LabSumAsm, @function
LabSumAsm:
    mv t0, a0
    li a0, 0
    beqz a1, .Lsum_done
.Lsum_next:
    ld t1, 0(t0)
    add a0, a0, t1
    addi t0, t0, 8
    addi a1, a1, -1
    bnez a1, .Lsum_next
.Lsum_done:
    ret
.size LabSumAsm, .-LabSumAsm
.section .note.GNU-stack,"",@progbits
