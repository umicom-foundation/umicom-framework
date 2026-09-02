/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_period.c
 *
 * PURPOSE:
 *   Exercise accounting period validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_period.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingAccountingPeriod v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_accounting_period_init(&v, "2026-08", (UmiFinancialDate){2026,8U,1U}, (UmiFinancialDate){2026,8U,31U}, UMI_ACCOUNTING_PERIOD_OPEN)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_accounting_accounting_period_open(&v)) return 2;
    return 0;
}
