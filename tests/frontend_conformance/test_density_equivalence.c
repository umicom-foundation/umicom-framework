/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_density_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for density-token equivalence scoring between renderer implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/density_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcDensityEquivalence e={28.0,29.0,2.0}; CHECK(umi_fc_density_equivalence_evaluate(&e)==1.0); e.actual=40.0; CHECK(umi_fc_density_equivalence_evaluate(&e)<1.0);
    return 0;
}
