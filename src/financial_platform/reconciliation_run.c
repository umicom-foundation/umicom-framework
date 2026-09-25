/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/reconciliation_run.c
 *
 * PURPOSE:
 *   Implement bounded tolerance matching, resolution and aged-break evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/reconciliation_run.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void umi_financial_reconciliation_run_init(
    UmiFinancialReconciliationRun *run,
    const char *runId)
{
    if (run == NULL) return;
    (void)memset(run, 0, sizeof(*run));
    if (runId != NULL)
        (void)snprintf(run->runId, sizeof(run->runId), "%s", runId);
    run->revision = 1U;
}

UmiStatus umi_financial_reconciliation_run_add(
    UmiFinancialReconciliationRun *run,
    const UmiFinancialReconciliationItem *item)
{
    UmiFinancialReconciliationItem copy;
    if (run == NULL || item == NULL || run->runId[0] == '\0' ||
        item->itemId[0] == '\0' || item->sourceId[0] == '\0' ||
        item->targetId[0] == '\0' || item->tolerance < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (run->count >= UMI_FINANCIAL_RECONCILIATION_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    copy = *item;
    copy.matched =
        fabs(copy.sourceValue - copy.targetValue) <= copy.tolerance;
    if (copy.matched) run->matchedCount += 1U;
    else run->breakCount += 1U;
    run->items[run->count++] = copy;
    run->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_reconciliation_run_resolve(
    UmiFinancialReconciliationRun *run,
    const char *itemId)
{
    size_t i;
    if (run == NULL || itemId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < run->count; ++i) {
        UmiFinancialReconciliationItem *item = &run->items[i];
        if (strcmp(item->itemId, itemId) == 0) {
            if (item->resolved) return UMI_STATUS_ALREADY_EXISTS;
            item->resolved = 1;
            if (!item->matched && run->breakCount > 0U) run->breakCount -= 1U;
            run->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

double umi_financial_reconciliation_run_coverage(
    const UmiFinancialReconciliationRun *run)
{
    size_t resolvedBreaks;
    if (run == NULL || run->count == 0U) return 0.0;
    resolvedBreaks = 0U;
    for (size_t i = 0U; i < run->count; ++i)
        if (!run->items[i].matched && run->items[i].resolved) resolvedBreaks += 1U;
    return ((double)(run->matchedCount + resolvedBreaks) /
            (double)run->count) * 100.0;
}

size_t umi_financial_reconciliation_run_aged_breaks(
    const UmiFinancialReconciliationRun *run,
    uint64_t nowMilliseconds,
    uint64_t ageThresholdMilliseconds)
{
    size_t i, count = 0U;
    if (run == NULL) return 0U;
    for (i = 0U; i < run->count; ++i) {
        const UmiFinancialReconciliationItem *item = &run->items[i];
        if (!item->matched && !item->resolved &&
            nowMilliseconds >= item->openedMilliseconds &&
            nowMilliseconds - item->openedMilliseconds >=
                ageThresholdMilliseconds) {
            count += 1U;
        }
    }
    return count;
}
