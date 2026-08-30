/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_update_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for automatic/manual update eligibility, metered-network and maintenance-window policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrUpdatePolicy p; umi_dr_update_policy_init(&p); CHECK(!umi_dr_update_policy_eligible(&p,false,true,false)); CHECK(umi_dr_update_policy_eligible(&p,false,true,true));
    return 0;
}
