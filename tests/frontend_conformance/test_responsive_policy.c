/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_responsive_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for responsive viewport policy selecting compact, standard and wide workstation modes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/responsive_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcResponsivePolicy p; umi_fc_responsive_policy_default(&p); CHECK(umi_fc_responsive_policy_classify(&p,600)==UMI_FC_VIEW_COMPACT); CHECK(umi_fc_responsive_policy_classify(&p,2200)==UMI_FC_VIEW_ULTRAWIDE);
    return 0;
}
