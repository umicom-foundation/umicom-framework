/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_clearing.c
 *
 * PURPOSE:
 *   Implement represent gross/net clearing values and participant count for payment rails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_clearing.h"
#include <string.h>
UmiStatus umi_payments_payment_clearing_init(UmiPaymentsPaymentClearing *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor,
    size_t participant_count,
    bool complete) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->gross_minor=gross_minor;
    value->net_minor=net_minor;
    value->participant_count=participant_count;
    value->complete=complete;
    return umi_payments_payment_clearing_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_clearing_valid(const UmiPaymentsPaymentClearing *value) {
    return value!=NULL && (value->gross_minor>=0 && umi_payments_abs_i64(value->net_minor)<=value->gross_minor && value->participant_count>0U);
}

bool umi_payments_payment_clearing_cleared(const UmiPaymentsPaymentClearing *value) {
    if(value==NULL) return (bool)0;
    return value->complete;
}
