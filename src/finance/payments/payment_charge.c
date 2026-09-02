/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_charge.c
 *
 * PURPOSE:
 *   Implement record payment fee and tax components without altering payment principal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_charge.h"
#include <string.h>
/*
 * Initialise payments payment charge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_charge_init(UmiPaymentsPaymentCharge *value,
    const char *id,
    const char *payment_id,
    int64_t fee_minor,
    int64_t tax_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->fee_minor=fee_minor;
    value->tax_minor=tax_minor;
    return umi_payments_payment_charge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment charge satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_charge_valid(const UmiPaymentsPaymentCharge *value) {
    return value!=NULL && (value->fee_minor>=0 && value->tax_minor>=0);
}

/*
 * Provide the payments payment charge total minor operation used by this module and its
 * client applications.
 */
int64_t umi_payments_payment_charge_total_minor(const UmiPaymentsPaymentCharge *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->fee_minor+value->tax_minor;
}
