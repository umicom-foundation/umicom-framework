/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_event.c
 *
 * PURPOSE:
 *   Exercise accounting event validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingAccountingEvent v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_accounting_event_init(&v, "evt-1", "CASH_RECEIPT", (UmiFinancialDate){2026,8U,25U}, 1000, "GBP")!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_accounting_accounting_event_absolute_minor(&v)!=1000) return 2;
    return 0;
}
