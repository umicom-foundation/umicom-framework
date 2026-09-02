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
/**
 * Represent the accounting reconciliation run data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingReconciliationRun {
    UmiFinancialId id;
    size_t total_items;
    size_t matched_items;
    int64_t residual_minor;
} UmiAccountingReconciliationRun;
/**
 * Initialise accounting reconciliation run from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_reconciliation_run_init(UmiAccountingReconciliationRun *value,
    const char *id,
    size_t total_items,
    size_t matched_items,
    int64_t residual_minor);
/**
 * Check that accounting reconciliation run satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_run_valid(const UmiAccountingReconciliationRun *value);
/**
 * Provide the accounting reconciliation run complete operation used by this module and its
 * client applications.
 */
bool umi_accounting_reconciliation_run_complete(const UmiAccountingReconciliationRun *value);
#ifdef __cplusplus
}
#endif
#endif
