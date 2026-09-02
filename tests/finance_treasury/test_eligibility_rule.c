/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_eligibility_rule.c
 *
 * PURPOSE:
 *   Exercise eligibility rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/eligibility_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryEligibilityRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_eligibility_rule_init(&v, "elig", 100, 365U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_treasury_eligibility_rule_usable(&v))return 2;
    return 0;
}
