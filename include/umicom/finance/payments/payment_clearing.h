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
typedef struct UmiPaymentsPaymentClearing {
    UmiFinancialId id;
    int64_t gross_minor;
    int64_t net_minor;
    size_t participant_count;
    bool complete;
} UmiPaymentsPaymentClearing;
UmiStatus umi_payments_payment_clearing_init(UmiPaymentsPaymentClearing *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor,
    size_t participant_count,
    bool complete);
bool umi_payments_payment_clearing_valid(const UmiPaymentsPaymentClearing *value);
bool umi_payments_payment_clearing_cleared(const UmiPaymentsPaymentClearing *value);
#ifdef __cplusplus
}
#endif
#endif
