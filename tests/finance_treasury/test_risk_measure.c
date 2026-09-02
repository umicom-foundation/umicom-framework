/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_measure.c
 *
 * PURPOSE:
 *   Exercise risk measure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_measure.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryRiskMeasure v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_measure_init(&v, "VAR", -250, 9900U, 1U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_risk_measure_absolute_value_minor(&v)!=250)return 2;
    return 0;
}
