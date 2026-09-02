/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_chart_of_accounts.c
 *
 * PURPOSE:
 *   Verify bounded chart of accounts insertion, duplicate prevention and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/chart_of_accounts.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAccountingChartOfAccounts r; UmiAccountingLedgerAccount item; umi_accounting_chart_of_accounts_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_ledger_account_init(&item, "1000", "Cash", UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_NORMAL_DEBIT, true)!=UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_chart_of_accounts_add(&r,&item)!=UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_chart_of_accounts_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_accounting_chart_of_accounts_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
