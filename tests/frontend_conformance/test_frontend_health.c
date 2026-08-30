/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_frontend_health.c
 *
 * PURPOSE:
 *   Focused regression coverage for aggregate frontend conformance health state and blocker counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/frontend_health.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcFrontendHealth h={10U,1U,0U,0U,0.95}; CHECK(umi_fc_frontend_health_outcome(&h)==UMI_FC_DEGRADED); CHECK(umi_fc_frontend_health_healthy(&h,0.9));
    return 0;
}
