/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/reconciliation_run.h
 *
 * PURPOSE:
 *   Execute bounded value reconciliation with tolerance, ageing and explicit
 *   break evidence across finance-domain records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_RUN_H
#define UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_RUN_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_RECONCILIATION_CAPACITY 512U

typedef struct UmiFinancialReconciliationItem {
    char itemId[64];
    char sourceId[64];
    char targetId[64];
    double sourceValue;
    double targetValue;
    double tolerance;
    uint64_t openedMilliseconds;
    int matched;
    int resolved;
} UmiFinancialReconciliationItem;

typedef struct UmiFinancialReconciliationRun {
    char runId[64];
    UmiFinancialReconciliationItem items[UMI_FINANCIAL_RECONCILIATION_CAPACITY];
    size_t count;
    size_t matchedCount;
    size_t breakCount;
    uint64_t revision;
} UmiFinancialReconciliationRun;

void umi_financial_reconciliation_run_init(
    UmiFinancialReconciliationRun *run,
    const char *runId);

UmiStatus umi_financial_reconciliation_run_add(
    UmiFinancialReconciliationRun *run,
    const UmiFinancialReconciliationItem *item);

UmiStatus umi_financial_reconciliation_run_resolve(
    UmiFinancialReconciliationRun *run,
    const char *itemId);

double umi_financial_reconciliation_run_coverage(
    const UmiFinancialReconciliationRun *run);

size_t umi_financial_reconciliation_run_aged_breaks(
    const UmiFinancialReconciliationRun *run,
    uint64_t nowMilliseconds,
    uint64_t ageThresholdMilliseconds);

#ifdef __cplusplus
}
#endif
#endif
