/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_result.c
 *
 * PURPOSE:
 *   Exercise risk result validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_result.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryRiskResult v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_result_init(&v, "risk-res", 100, 175, 200) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_treasury_risk_result_within_limit(&v))return 2;
    return 0;
}
