/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_keyboard_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for keyboard command coverage scoring and mandatory shortcut detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/keyboard_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcKeyboardEquivalence x={10U,9U,0U}; CHECK(umi_fc_keyboard_equivalence_evaluate(&x)>0.89); CHECK(umi_fc_keyboard_equivalence_passes(&x,0.85)); x.mandatory_missing=1U; CHECK(!umi_fc_keyboard_equivalence_passes(&x,0.85));
    return 0;
}
