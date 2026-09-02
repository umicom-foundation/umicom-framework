/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_counterparty_exposure.c
 *
 * PURPOSE:
 *   Exercise counterparty exposure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/counterparty_exposure.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryCounterpartyExposure v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_counterparty_exposure_init(&v, "cpty", 300, 200) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_counterparty_exposure_total_minor(&v)!=500)return 2;
    return 0;
}
