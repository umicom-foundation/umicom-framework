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
UmiStatus umi_payments_payment_batch_init(UmiPaymentsPaymentBatch *value,
    const char *id,
    const char *currency_code,
    size_t payment_count,
    int64_t total_minor,
    bool approved) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_currency_from_code(currency_code,&value->currency);
    if(rc!=UMI_STATUS_OK)return rc;
    value->payment_count=payment_count;
    value->total_minor=total_minor;
    value->approved=approved;
    return umi_payments_payment_batch_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_batch_valid(const UmiPaymentsPaymentBatch *value) {
    return value!=NULL && (value->payment_count>0U && value->payment_count<=UMI_PAYMENTS_MAX_ITEMS && value->total_minor>0);
}

bool umi_payments_payment_batch_releasable(const UmiPaymentsPaymentBatch *value) {
    if(value==NULL) return (bool)0;
    return value->approved;
}
