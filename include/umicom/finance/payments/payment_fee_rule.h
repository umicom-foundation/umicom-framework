/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_fee_rule.h
 *
 * PURPOSE:
 *   Calculate fixed plus proportional payment fees in minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_FEE_RULE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_FEE_RULE_H
#include "umicom/finance/payments/types.h"
#include "umicom/finance/money_rate.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment fee rule data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentFeeRule {
    UmiFinancialId id;
    int64_t fixed_fee_minor;
    uint32_t variable_fee_bps;
    int64_t maximum_fee_minor;
} UmiPaymentsPaymentFeeRule;
/**
 * Initialise payments payment fee rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_fee_rule_init(UmiPaymentsPaymentFeeRule *value,
    const char *id,
    int64_t fixed_fee_minor,
    uint32_t variable_fee_bps,
    int64_t maximum_fee_minor);
/**
 * Check that payments payment fee rule satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_fee_rule_valid(const UmiPaymentsPaymentFeeRule *value);
/**
 * Provide the payments payment fee rule fee for 10000 minor operation used by this module
 * and its client applications.
 */
int64_t umi_payments_payment_fee_rule_fee_for_10000_minor(const UmiPaymentsPaymentFeeRule *value);
/** Calculate min(maximum, fixed + rounded proportional fee) for any nonnegative
 * principal. All amounts use the same caller-supplied minor-unit scale. The cap
 * is inclusive; zero means a zero fee, not "unlimited". Failure leaves outFee
 * unchanged. The rule must have a valid ID, 0..10000 bps, and maximum >= fixed.
 * This calculation does not change a payment or reserve funds. */
UmiStatus umi_payments_payment_fee_rule_calculate(const UmiPaymentsPaymentFeeRule *value,
    int64_t principalMinor, UmiMoneyRounding rounding, int64_t *outFee);
/* The legacy 10000-minor wrapper retains truncation and returns zero on invalid
 * input. Use the checked calculation to distinguish invalid input from no fee.
 * Initialisation now leaves the destination unchanged on failure. */
#ifdef __cplusplus
}
#endif
#endif
