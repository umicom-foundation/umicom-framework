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
int main(void) {
    UmiAccountingLedgerAccount v;
    if(umi_accounting_ledger_account_init(&v, "1000", "Cash", UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_NORMAL_DEBIT, true)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_ledger_account_valid(&v)) return 2;
    return 0;
}
