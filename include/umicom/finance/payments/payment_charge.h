/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_charge.h
 *
 * PURPOSE:
 *   Record payment fee and tax components without altering payment principal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_CHARGE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_CHARGE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment charge data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentCharge {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    int64_t fee_minor;
    int64_t tax_minor;
} UmiPaymentsPaymentCharge;
/**
 * Initialise payments payment charge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_charge_init(UmiPaymentsPaymentCharge *value,
    const char *id,
    const char *payment_id,
    int64_t fee_minor,
    int64_t tax_minor);
/**
 * Check that payments payment charge satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_charge_valid(const UmiPaymentsPaymentCharge *value);
/**
 * Provide the payments payment charge total minor operation used by this module and its
 * client applications.
 */
int64_t umi_payments_payment_charge_total_minor(const UmiPaymentsPaymentCharge *value);
#ifdef __cplusplus
}
#endif
#endif
