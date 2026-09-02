/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/reconciliation_item.h
 *
 * PURPOSE:
 *   Compare external and ledger values and expose deterministic difference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_RECONCILIATION_ITEM_H
#define UMICOM_FINANCE_ACCOUNTING_RECONCILIATION_ITEM_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting reconciliation item data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingReconciliationItem {
    UmiFinancialId id;
    int64_t external_minor;
    int64_t ledger_minor;
    int64_t tolerance_minor;
} UmiAccountingReconciliationItem;
/**
 * Initialise accounting reconciliation item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_reconciliation_item_init(UmiAccountingReconciliationItem *value,
    const char *id,
    int64_t external_minor,
    int64_t ledger_minor,
    int64_t tolerance_minor);
/**
 * Check that accounting reconciliation item satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_item_valid(const UmiAccountingReconciliationItem *value);
/**
 * Provide the accounting reconciliation item matched operation used by this module and its
 * client applications.
 */
bool umi_accounting_reconciliation_item_matched(const UmiAccountingReconciliationItem *value);
#ifdef __cplusplus
}
#endif
#endif
