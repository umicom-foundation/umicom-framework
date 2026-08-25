/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_subledger.c
 *
 * PURPOSE:
 *   Exercise subledger validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/subledger.h"
int main(void) {
    UmiAccountingSubledger v;
    if(umi_accounting_subledger_init(&v, "sub-1", "1200", 3000, 500)!=UMI_STATUS_OK) return 1;
    if(umi_accounting_subledger_net_minor(&v)!=2500) return 2;
    return 0;
}
