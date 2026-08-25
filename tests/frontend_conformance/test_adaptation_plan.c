/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_adaptation_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for reviewable frontend adaptation operations for unsupported or constrained capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/adaptation_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcAdaptationPlan p; umi_fc_adaptation_plan_build(7U,3U,4U,&p); CHECK(p.operations==3U); CHECK(umi_fc_adaptation_plan_outcome(&p)==UMI_FC_DEGRADED);
    return 0;
}
