/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/spin_hint.c
 *
 * PURPOSE:
 *   Route bounded worker spin hints to audited assembly where available and a
 *   standards-compliant compiler fence elsewhere.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/spin_hint.h"

#include <stdatomic.h>

#if defined(UMICOM_DEVELOPER_SPIN_X86_64_ASM)
void umi_developer_spin_hint_asm(void);
#endif

void umi_developer_spin_hint(unsigned iterations)
{
    unsigned index;
    for (index = 0U; index < iterations; ++index) {
#if defined(UMICOM_DEVELOPER_SPIN_X86_64_ASM)
        umi_developer_spin_hint_asm();
#else
        /* A signal fence is intentionally side-effect free at runtime while
         * preventing the compiler from deleting a bounded cooperative loop. */
        atomic_signal_fence(memory_order_seq_cst);
#endif
    }
}
