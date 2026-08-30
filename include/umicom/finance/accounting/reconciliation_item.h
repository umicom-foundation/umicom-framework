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
typedef struct UmiAccountingReconciliationItem {
    UmiFinancialId id;
    int64_t external_minor;
    int64_t ledger_minor;
    int64_t tolerance_minor;
} UmiAccountingReconciliationItem;
UmiStatus umi_accounting_reconciliation_item_init(UmiAccountingReconciliationItem *value,
    const char *id,
    int64_t external_minor,
    int64_t ledger_minor,
    int64_t tolerance_minor);
bool umi_accounting_reconciliation_item_valid(const UmiAccountingReconciliationItem *value);
bool umi_accounting_reconciliation_item_matched(const UmiAccountingReconciliationItem *value);
#ifdef __cplusplus
}
#endif
#endif
