/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/asm/spin_hint_x86_64.S
 *
 * PURPOSE:
 *   Implement the spin hint x86 64 behaviour used by its public contract and
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
 * File: src/developer/asm/spin_hint_x86_64.S
 *
 * PURPOSE:
 *   Issue the x86 PAUSE hint for short, bounded developer-runtime wait loops.
 *   The implementation is ABI-neutral because it accepts no arguments and
 *   returns no value; C retains iteration policy and the portable fallback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
    .text
    .globl umi_developer_spin_hint_asm
#ifdef _WIN32
    .def umi_developer_spin_hint_asm; .scl 2; .type 32; .endef
#else
    .type umi_developer_spin_hint_asm, @function
#endif
/* Yield execution resources briefly while the surrounding C code owns the wait policy. */
umi_developer_spin_hint_asm:
    pause
    ret
#ifndef _WIN32
    .size umi_developer_spin_hint_asm, .-umi_developer_spin_hint_asm
    .section .note.GNU-stack,"",@progbits
#endif
