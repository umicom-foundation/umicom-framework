/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_reconciliation_run.c
 *
 * PURPOSE:
 *   Verify tolerance matching, break ageing, resolution and coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/financial_platform/reconciliation_run.h"

int main(void)
{
    UmiFinancialReconciliationRun run;
    UmiFinancialReconciliationItem matched = {0};
    UmiFinancialReconciliationItem broken = {0};

    umi_financial_reconciliation_run_init(&run, "run-1");
    (void)strcpy(matched.itemId, "item-1");
    (void)strcpy(matched.sourceId, "source-1");
    (void)strcpy(matched.targetId, "target-1");
    matched.sourceValue = 100.0;
    matched.targetValue = 100.2;
    matched.tolerance = 0.5;
    matched.openedMilliseconds = 1000U;
    broken = matched;
    (void)strcpy(broken.itemId, "item-2");
    broken.targetValue = 102.0;

    assert(umi_financial_reconciliation_run_add(&run, &matched) == UMI_STATUS_OK);
    assert(umi_financial_reconciliation_run_add(&run, &broken) == UMI_STATUS_OK);
    assert(run.matchedCount == 1U);
    assert(run.breakCount == 1U);
    assert(umi_financial_reconciliation_run_aged_breaks(
               &run, 3000U, 1000U) == 1U);
    assert(umi_financial_reconciliation_run_resolve(&run, "item-2") ==
           UMI_STATUS_OK);
    assert(umi_financial_reconciliation_run_coverage(&run) > 99.9);
    return 0;
}
