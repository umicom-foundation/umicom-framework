/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_ledger_account.c
 *
 * PURPOSE:
 *   Exercise ledger account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingLedgerAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_ledger_account_init(&v, "1000", "Cash", UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_NORMAL_DEBIT, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_ledger_account_valid(&v)) return 2;
    return 0;
}
