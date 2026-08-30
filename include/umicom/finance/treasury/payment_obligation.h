/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/payment_obligation.h
 *
 * PURPOSE:
 *   Represent a dated treasury payment obligation and outstanding amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_PAYMENT_OBLIGATION_H
#define UMICOM_FINANCE_TREASURY_PAYMENT_OBLIGATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryPaymentObligation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t due_epoch_millis;
    int64_t amount_minor;
    int64_t paid_minor;
} UmiTreasuryPaymentObligation;
UmiStatus umi_treasury_payment_obligation_init(UmiTreasuryPaymentObligation *value,
    const char *id,
    int64_t due_epoch_millis,
    int64_t amount_minor,
    int64_t paid_minor);
bool umi_treasury_payment_obligation_valid(const UmiTreasuryPaymentObligation *value);
int64_t umi_treasury_payment_obligation_outstanding_minor(const UmiTreasuryPaymentObligation *value);
#ifdef __cplusplus
}
#endif
#endif
