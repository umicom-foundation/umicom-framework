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
int main(void) {
    UmiAccountingReconciliationRun v;
    if(umi_accounting_reconciliation_run_init(&v, "run-1", 10U, 10U, 0)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_reconciliation_run_complete(&v)) return 2;
    return 0;
}
