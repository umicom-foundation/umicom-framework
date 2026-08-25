/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_chart_of_accounts.c
 *
 * PURPOSE:
 *   Verify bounded chart of accounts insertion, duplicate prevention and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/chart_of_accounts.h"
int main(void) { UmiAccountingChartOfAccounts r; UmiAccountingLedgerAccount item; umi_accounting_chart_of_accounts_init(&r);
    if(umi_accounting_ledger_account_init(&item, "1000", "Cash", UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_NORMAL_DEBIT, true)!=UMI_STATUS_OK) return 1;
    if(umi_accounting_chart_of_accounts_add(&r,&item)!=UMI_STATUS_OK) return 2;
    if(umi_accounting_chart_of_accounts_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    if(umi_accounting_chart_of_accounts_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
