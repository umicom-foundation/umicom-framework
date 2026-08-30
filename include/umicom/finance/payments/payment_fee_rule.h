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
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentFeeRule {
    UmiFinancialId id;
    int64_t fixed_fee_minor;
    uint32_t variable_fee_bps;
    int64_t maximum_fee_minor;
} UmiPaymentsPaymentFeeRule;
UmiStatus umi_payments_payment_fee_rule_init(UmiPaymentsPaymentFeeRule *value,
    const char *id,
    int64_t fixed_fee_minor,
    uint32_t variable_fee_bps,
    int64_t maximum_fee_minor);
bool umi_payments_payment_fee_rule_valid(const UmiPaymentsPaymentFeeRule *value);
int64_t umi_payments_payment_fee_rule_fee_for_10000_minor(const UmiPaymentsPaymentFeeRule *value);
#ifdef __cplusplus
}
#endif
#endif
