/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_funding_requirement.c
 *
 * PURPOSE:
 *   Exercise funding requirement validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_requirement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryFundingRequirement v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_funding_requirement_init(&v, "req", 1000, 725) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_funding_requirement_shortfall_minor(&v)!=275)return 2;
    return 0;
}
