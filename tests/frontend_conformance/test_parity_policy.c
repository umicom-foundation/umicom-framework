/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_parity_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for minimum parity thresholds and blocker policy for release decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/parity_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcParityPolicy p; umi_fc_parity_policy_default(&p); CHECK(umi_fc_parity_policy_accepts(&p,0.95,0.98,0.94,1U,0U)); CHECK(!umi_fc_parity_policy_accepts(&p,1.0,1.0,1.0,0U,1U));
    return 0;
}
