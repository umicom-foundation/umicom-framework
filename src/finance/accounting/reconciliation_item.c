/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/reconciliation_item.c
 *
 * PURPOSE:
 *   Implement compare external and ledger values and expose deterministic difference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_item.h"
#include <string.h>

/* Preserve the former arithmetic for review. Complete validation and checked
 * exact arithmetic replace unchecked subtraction; valid values retain their
 * established result without saturation-based matching. */
#if 0
/*
 * Initialise accounting reconciliation item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_reconciliation_item_init(UmiAccountingReconciliationItem *value,
    const char *id,
    int64_t external_minor,
    int64_t ledger_minor,
    int64_t tolerance_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->external_minor=external_minor;
    value->ledger_minor=ledger_minor;
    value->tolerance_minor=tolerance_minor;
    return umi_accounting_reconciliation_item_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting reconciliation item satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_item_valid(const UmiAccountingReconciliationItem *value) {
    return value!=NULL && (value->tolerance_minor>=0);
}

/*
 * Provide the accounting reconciliation item matched operation used by this module and its
 * client applications.
 */
bool umi_accounting_reconciliation_item_matched(const UmiAccountingReconciliationItem *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return umi_accounting_abs_i64(value->external_minor-value->ledger_minor)<=value->tolerance_minor;
}

#endif

/* Reconciliation must compare the exact distance even when signed subtraction
 * cannot represent it. Unsigned subtraction is defined modulo 2^64; ordering
 * chooses the nonnegative mathematical distance, which fits uint64_t. */
UmiStatus umi_accounting_reconciliation_item_init(UmiAccountingReconciliationItem *value,
    const char *id, int64_t external_minor, int64_t ledger_minor, int64_t tolerance_minor)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiAccountingReconciliationItem next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    next.external_minor = external_minor;
    next.ledger_minor = ledger_minor;
    next.tolerance_minor = tolerance_minor;
    if (!umi_accounting_reconciliation_item_valid(&next)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *value = next;
    return UMI_STATUS_OK;
}

bool umi_accounting_reconciliation_item_valid(const UmiAccountingReconciliationItem *value)
{
    return value != NULL && umi_financial_id_is_valid(&value->id) && value->tolerance_minor >= 0;
}

UmiStatus umi_accounting_reconciliation_item_distance_minor_checked(
    const UmiAccountingReconciliationItem *value, uint64_t *out_distance)
{
    if (out_distance == NULL || !umi_accounting_reconciliation_item_valid(value)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_distance = value->external_minor >= value->ledger_minor ?
        (uint64_t)value->external_minor - (uint64_t)value->ledger_minor :
        (uint64_t)value->ledger_minor - (uint64_t)value->external_minor;
    return UMI_STATUS_OK;
}

UmiStatus umi_accounting_reconciliation_item_difference_minor_checked(
    const UmiAccountingReconciliationItem *value, int64_t *out_difference)
{
    if (out_difference == NULL || !umi_accounting_reconciliation_item_valid(value)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((value->ledger_minor > 0 && value->external_minor < INT64_MIN + value->ledger_minor) ||
        (value->ledger_minor < 0 && value->external_minor > INT64_MAX + value->ledger_minor)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out_difference = value->external_minor - value->ledger_minor;
    return UMI_STATUS_OK;
}

bool umi_accounting_reconciliation_item_matched(const UmiAccountingReconciliationItem *value)
{
    uint64_t distance = 0U;
    return umi_accounting_reconciliation_item_distance_minor_checked(value, &distance) == UMI_STATUS_OK &&
        distance <= (uint64_t)value->tolerance_minor;
}
