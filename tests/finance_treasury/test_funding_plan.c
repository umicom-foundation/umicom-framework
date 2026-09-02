/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_funding_plan.c
 *
 * PURPOSE:
 *   Exercise funding plan validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryFundingPlan v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_funding_plan_init(&v, "plan", 500, 350) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_funding_plan_remaining_minor(&v)!=150)return 2;
    return 0;
}
