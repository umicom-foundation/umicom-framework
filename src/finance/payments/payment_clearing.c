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
/*
 * Initialise payments payment clearing from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_clearing_init(UmiPaymentsPaymentClearing *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor,
    size_t participant_count,
    bool complete) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->gross_minor=gross_minor;
    value->net_minor=net_minor;
    value->participant_count=participant_count;
    value->complete=complete;
    return umi_payments_payment_clearing_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment clearing satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_clearing_valid(const UmiPaymentsPaymentClearing *value) {
    return value!=NULL && (value->gross_minor>=0 && umi_payments_abs_i64(value->net_minor)<=value->gross_minor && value->participant_count>0U);
}

/*
 * Provide the payments payment clearing cleared operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_clearing_cleared(const UmiPaymentsPaymentClearing *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->complete;
}
