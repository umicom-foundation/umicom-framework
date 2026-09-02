/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_reconciliation_run.c
 *
 * PURPOSE:
 *   Exercise reconciliation run validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_run.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingReconciliationRun v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_reconciliation_run_init(&v, "run-1", 10U, 10U, 0)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_accounting_reconciliation_run_complete(&v)) return 2;
    return 0;
}
