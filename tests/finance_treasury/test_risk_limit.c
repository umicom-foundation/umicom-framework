/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_limit.c
 *
 * PURPOSE:
 *   Exercise risk limit validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_limit.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryRiskLimit v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_limit_init(&v, "limit", 1000, 800) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_risk_limit_buffer_minor(&v)!=200)return 2;
    return 0;
}
