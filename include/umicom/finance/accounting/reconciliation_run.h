/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/reconciliation_run.h
 *
 * PURPOSE:
 *   Summarise reconciliation matched/unmatched item counts and residual difference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_RECONCILIATION_RUN_H
#define UMICOM_FINANCE_ACCOUNTING_RECONCILIATION_RUN_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingReconciliationRun {
    UmiFinancialId id;
    size_t total_items;
    size_t matched_items;
    int64_t residual_minor;
} UmiAccountingReconciliationRun;
UmiStatus umi_accounting_reconciliation_run_init(UmiAccountingReconciliationRun *value,
    const char *id,
    size_t total_items,
    size_t matched_items,
    int64_t residual_minor);
bool umi_accounting_reconciliation_run_valid(const UmiAccountingReconciliationRun *value);
bool umi_accounting_reconciliation_run_complete(const UmiAccountingReconciliationRun *value);
#ifdef __cplusplus
}
#endif
#endif
