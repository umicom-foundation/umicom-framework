/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_selection_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for selection-mode parity scoring and graceful-degradation classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/selection_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcSelectionEquivalence x={7U,3U}; CHECK(umi_fc_selection_equivalence_evaluate(&x)>0.0); CHECK(umi_fc_selection_equivalence_outcome(&x)==UMI_FC_FAIL);
    return 0;
}
