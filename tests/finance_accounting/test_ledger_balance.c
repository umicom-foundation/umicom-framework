/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_ledger_balance.c
 *
 * PURPOSE:
 *   Exercise ledger balance validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_balance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingLedgerBalance v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_ledger_balance_init(&v, "bal-1", "1000", UMI_ACCOUNTING_NORMAL_DEBIT, 1200, 200)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_accounting_ledger_balance_net_minor(&v)!=1000) return 2;
    return 0;
}
