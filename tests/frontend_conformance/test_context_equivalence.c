/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_context_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for context propagation parity and directionality scoring across frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/context_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcContextEquivalence x={3U,3U,1U}; CHECK(umi_fc_context_equivalence_evaluate(&x)>0.0); CHECK(umi_fc_context_equivalence_outcome(&x)==UMI_FC_DEGRADED);
    return 0;
}
