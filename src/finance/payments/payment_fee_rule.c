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
#include "umicom/finance/identifier.h"
/*
 * Initialise payments payment fee rule from caller-provided values so later operations
 * receive a known state.
 */
/* Build a candidate before publication so a failed price-rule edit cannot damage the previous rule. The previous implementation remains for engineering review. */
#if 0
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
#endif
UmiStatus umi_payments_payment_fee_rule_init(UmiPaymentsPaymentFeeRule *value,
    const char *id, int64_t fixed_fee_minor, uint32_t variable_fee_bps, int64_t maximum_fee_minor)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiPaymentsPaymentFeeRule candidate = {0};
    UmiStatus status = umi_payments_id_assign(&candidate.id, id);
    if (status != UMI_STATUS_OK) return status;
    candidate.fixed_fee_minor = fixed_fee_minor;
    candidate.variable_fee_bps = variable_fee_bps;
    candidate.maximum_fee_minor = maximum_fee_minor;
    if (!umi_payments_payment_fee_rule_valid(&candidate)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = candidate;
    return UMI_STATUS_OK;
}
/*
 * Check that payments payment fee rule satisfies its contract before another service
 * relies on it.
 */
/* Price quotes require a bounded rule identity as well as nonnegative, internally consistent amounts. The previous implementation remains for engineering review. */
#if 0
bool umi_payments_payment_fee_rule_valid(const UmiPaymentsPaymentFeeRule *value) {
    return value!=NULL && (value->fixed_fee_minor>=0 && value->variable_fee_bps<=10000U && value->maximum_fee_minor>=value->fixed_fee_minor);
}
#endif
bool umi_payments_payment_fee_rule_valid(const UmiPaymentsPaymentFeeRule *value)
{
    return value != NULL && umi_financial_id_valid(&value->id) && value->fixed_fee_minor >= 0 &&
        value->variable_fee_bps <= 10000U && value->maximum_fee_minor >= value->fixed_fee_minor;
}

/*
 * Provide the payments payment fee rule fee for 10000 minor operation used by this module
 * and its client applications.
 */
/* Route the legacy convenience calculation through shared checked arithmetic; applying the cap before addition prevents signed overflow. The previous implementation remains for engineering review. */
#if 0
int64_t umi_payments_payment_fee_rule_fee_for_10000_minor(const UmiPaymentsPaymentFeeRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return ((value->fixed_fee_minor + ((10000LL*(int64_t)value->variable_fee_bps)/10000LL)) > value->maximum_fee_minor) ? value->maximum_fee_minor : (value->fixed_fee_minor + ((10000LL*(int64_t)value->variable_fee_bps)/10000LL));
}
#endif
int64_t umi_payments_payment_fee_rule_fee_for_10000_minor(const UmiPaymentsPaymentFeeRule *value)
{
    int64_t fee = 0;
    (void)umi_payments_payment_fee_rule_calculate(value, 10000, UMI_MONEY_TOWARD_ZERO, &fee);
    return fee;
}


UmiStatus umi_payments_payment_fee_rule_calculate(const UmiPaymentsPaymentFeeRule *value,
    int64_t principalMinor, UmiMoneyRounding rounding, int64_t *outFee)
{
    if (outFee == NULL || principalMinor < 0 || !umi_payments_payment_fee_rule_valid(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    int64_t variable;
    UmiStatus status = UmiMinorApplyBasisPoints(principalMinor, value->variable_fee_bps, rounding, &variable);
    if (status != UMI_STATUS_OK) return status;
    int64_t headroom = value->maximum_fee_minor - value->fixed_fee_minor;
    *outFee = value->fixed_fee_minor + (variable > headroom ? headroom : variable);
    return UMI_STATUS_OK;
}
