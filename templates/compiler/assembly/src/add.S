/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/assembly/src/add.S
 *
 * PURPOSE:
 *   Provide reusable add source for projects created with Umicom tooling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom x86-64 assembly template | Sammy Hegab | Umicom Foundation | MIT */
.text
.globl umicom_add
/* Add the two ABI integer arguments and return the result in the standard register. */
umicom_add:
    movl %edi, %eax
    addl %esi, %eax
    ret
