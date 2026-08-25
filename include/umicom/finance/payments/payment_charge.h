/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_charge.h
 *
 * PURPOSE:
 *   Record payment fee and tax components without altering payment principal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_CHARGE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_CHARGE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentCharge {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    int64_t fee_minor;
    int64_t tax_minor;
} UmiPaymentsPaymentCharge;
UmiStatus umi_payments_payment_charge_init(UmiPaymentsPaymentCharge *value,
    const char *id,
    const char *payment_id,
    int64_t fee_minor,
    int64_t tax_minor);
bool umi_payments_payment_charge_valid(const UmiPaymentsPaymentCharge *value);
int64_t umi_payments_payment_charge_total_minor(const UmiPaymentsPaymentCharge *value);
#ifdef __cplusplus
}
#endif
#endif
