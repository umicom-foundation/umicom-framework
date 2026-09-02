/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_fee_rule.c
 *
 * PURPOSE:
 *   Implement calculate fixed plus proportional payment fees in minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_fee_rule.h"
#include <string.h>
/*
 * Initialise payments payment fee rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_fee_rule_init(UmiPaymentsPaymentFeeRule *value,
    const char *id,
    int64_t fixed_fee_minor,
    uint32_t variable_fee_bps,
    int64_t maximum_fee_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->fixed_fee_minor=fixed_fee_minor;
    value->variable_fee_bps=variable_fee_bps;
    value->maximum_fee_minor=maximum_fee_minor;
    return umi_payments_payment_fee_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment fee rule satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_fee_rule_valid(const UmiPaymentsPaymentFeeRule *value) {
    return value!=NULL && (value->fixed_fee_minor>=0 && value->variable_fee_bps<=10000U && value->maximum_fee_minor>=value->fixed_fee_minor);
}

/*
 * Provide the payments payment fee rule fee for 10000 minor operation used by this module
 * and its client applications.
 */
int64_t umi_payments_payment_fee_rule_fee_for_10000_minor(const UmiPaymentsPaymentFeeRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return ((value->fixed_fee_minor + ((10000LL*(int64_t)value->variable_fee_bps)/10000LL)) > value->maximum_fee_minor) ? value->maximum_fee_minor : (value->fixed_fee_minor + ((10000LL*(int64_t)value->variable_fee_bps)/10000LL));
}
