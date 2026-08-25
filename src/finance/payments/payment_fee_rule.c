/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_fee_rule.c
 *
 * PURPOSE:
 *   Implement calculate fixed plus proportional payment fees in minor units.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_fee_rule.h"
#include <string.h>
UmiStatus umi_payments_payment_fee_rule_init(UmiPaymentsPaymentFeeRule *value,
    const char *id,
    int64_t fixed_fee_minor,
    uint32_t variable_fee_bps,
    int64_t maximum_fee_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->fixed_fee_minor=fixed_fee_minor;
    value->variable_fee_bps=variable_fee_bps;
    value->maximum_fee_minor=maximum_fee_minor;
    return umi_payments_payment_fee_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_fee_rule_valid(const UmiPaymentsPaymentFeeRule *value) {
    return value!=NULL && (value->fixed_fee_minor>=0 && value->variable_fee_bps<=10000U && value->maximum_fee_minor>=value->fixed_fee_minor);
}

int64_t umi_payments_payment_fee_rule_fee_for_10000_minor(const UmiPaymentsPaymentFeeRule *value) {
    if(value==NULL) return (int64_t)0;
    return ((value->fixed_fee_minor + ((10000LL*(int64_t)value->variable_fee_bps)/10000LL)) > value->maximum_fee_minor) ? value->maximum_fee_minor : (value->fixed_fee_minor + ((10000LL*(int64_t)value->variable_fee_bps)/10000LL));
}
