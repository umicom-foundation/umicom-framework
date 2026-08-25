/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_trial_balance.c
 *
 * PURPOSE:
 *   Exercise trial balance validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/trial_balance.h"
int main(void) {
    UmiAccountingTrialBalance v;
    if(umi_accounting_trial_balance_init(&v, "tb-1", 5000, 5000, 12U)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_trial_balance_balanced(&v)) return 2;
    return 0;
}
