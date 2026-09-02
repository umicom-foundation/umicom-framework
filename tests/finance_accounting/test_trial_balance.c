/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_trial_balance.c
 *
 * PURPOSE:
 *   Exercise trial balance validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/trial_balance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingTrialBalance v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_trial_balance_init(&v, "tb-1", 5000, 5000, 12U)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_accounting_trial_balance_balanced(&v)) return 2;
    return 0;
}
