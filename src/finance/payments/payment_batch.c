/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_batch.c
 *
 * PURPOSE:
 *   Implement represent approved same-currency payment batches using bounded aggregate evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_batch.h"
#include <string.h>
/*
 * Initialise payments payment batch from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_batch_init(UmiPaymentsPaymentBatch *value,
    const char *id,
    const char *currency_code,
    size_t payment_count,
    int64_t total_minor,
    bool approved) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_currency_from_code(currency_code,&value->currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->payment_count=payment_count;
    value->total_minor=total_minor;
    value->approved=approved;
    return umi_payments_payment_batch_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment batch satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_batch_valid(const UmiPaymentsPaymentBatch *value) {
    return value!=NULL && (value->payment_count>0U && value->payment_count<=UMI_PAYMENTS_MAX_ITEMS && value->total_minor>0);
}

/*
 * Provide the payments payment batch releasable operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_batch_releasable(const UmiPaymentsPaymentBatch *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->approved;
}
