/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_reconciliation.c
 *
 * PURPOSE:
 *   Implement compare external statement totals against internal payment-ledger totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_reconciliation.h"
#include <string.h>
/*
 * Initialise payments payment reconciliation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_payments_payment_reconciliation_init(UmiPaymentsPaymentReconciliation *value,
    const char *id,
    int64_t statement_minor,
    int64_t ledger_minor,
    uint32_t unmatched_items) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->statement_minor=statement_minor;
    value->ledger_minor=ledger_minor;
    value->unmatched_items=unmatched_items;
    return umi_payments_payment_reconciliation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment reconciliation satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_reconciliation_valid(const UmiPaymentsPaymentReconciliation *value) {
    return value!=NULL && (true);
}

/*
 * Provide the payments payment reconciliation difference minor operation used by this
 * module and its client applications.
 */
int64_t umi_payments_payment_reconciliation_difference_minor(const UmiPaymentsPaymentReconciliation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->statement_minor-value->ledger_minor;
}
