/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_focus_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for focus traversal equivalence scoring across native and browser renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/focus_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcFocusEquivalence x={10U,9U,0U}; CHECK(umi_fc_focus_equivalence_evaluate(&x)>0.89); CHECK(umi_fc_focus_equivalence_passes(&x,0.85)); x.traps_missing=1U; CHECK(!umi_fc_focus_equivalence_passes(&x,0.85));
    return 0;
}
