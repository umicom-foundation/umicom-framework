/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_event_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for event-family parity scoring for pointer, keyboard, input, focus and drag/drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcEventEquivalence x={7U,3U}; CHECK(umi_fc_event_equivalence_evaluate(&x)>0.0); CHECK(umi_fc_event_equivalence_outcome(&x)==UMI_FC_FAIL);
    return 0;
}
