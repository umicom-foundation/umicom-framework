/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/reconciliation_rule.h
 *
 * PURPOSE:
 *   Define reconciliation tolerance and automatic matching policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_RECONCILIATION_RULE_H
#define UMICOM_FINANCE_ACCOUNTING_RECONCILIATION_RULE_H
#include "umicom/finance/accounting/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting reconciliation rule data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingReconciliationRule {
    UmiFinancialId id;
    int64_t tolerance_minor;
    bool auto_match;
} UmiAccountingReconciliationRule;
/**
 * Initialise accounting reconciliation rule from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_reconciliation_rule_init(UmiAccountingReconciliationRule *value,
    const char *id,
    int64_t tolerance_minor,
    bool auto_match);
/**
 * Check that accounting reconciliation rule satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_rule_valid(const UmiAccountingReconciliationRule *value);
/**
 * Provide the accounting reconciliation rule automatic operation used by this module and
 * its client applications.
 */
bool umi_accounting_reconciliation_rule_automatic(const UmiAccountingReconciliationRule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_accounting_reconciliation_rule_archive_encode(const UmiAccountingReconciliationRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_accounting_reconciliation_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiAccountingReconciliationRule *value);

#ifdef __cplusplus
}
#endif
#endif
