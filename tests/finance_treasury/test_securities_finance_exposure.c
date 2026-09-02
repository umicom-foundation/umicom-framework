/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_securities_finance_exposure.c
 *
 * PURPOSE:
 *   Exercise securities finance exposure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_finance_exposure.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasurySecuritiesFinanceExposure v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_securities_finance_exposure_init(&v, "sfe", 1000, 800) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_securities_finance_exposure_unsecured_minor(&v)!=200)return 2;
    return 0;
}
