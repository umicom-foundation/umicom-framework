/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_clearing.h
 *
 * PURPOSE:
 *   Represent gross/net clearing values and participant count for payment rails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_CLEARING_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_CLEARING_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment clearing data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentClearing {
    UmiFinancialId id;
    int64_t gross_minor;
    int64_t net_minor;
    size_t participant_count;
    bool complete;
} UmiPaymentsPaymentClearing;
/**
 * Initialise payments payment clearing from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_clearing_init(UmiPaymentsPaymentClearing *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor,
    size_t participant_count,
    bool complete);
/**
 * Check that payments payment clearing satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_clearing_valid(const UmiPaymentsPaymentClearing *value);
/**
 * Provide the payments payment clearing cleared operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_clearing_cleared(const UmiPaymentsPaymentClearing *value);
#ifdef __cplusplus
}
#endif
#endif
