/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_ledger_balance.c
 *
 * PURPOSE:
 *   Exercise ledger balance validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_balance.h"
int main(void) {
    UmiAccountingLedgerBalance v;
    if(umi_accounting_ledger_balance_init(&v, "bal-1", "1000", UMI_ACCOUNTING_NORMAL_DEBIT, 1200, 200)!=UMI_STATUS_OK) return 1;
    if(umi_accounting_ledger_balance_net_minor(&v)!=1000) return 2;
    return 0;
}
