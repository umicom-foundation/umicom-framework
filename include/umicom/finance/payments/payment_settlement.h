/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_settlement.h
 *
 * PURPOSE:
 *   Represent payment settlement reference, amount and final settlement evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_SETTLEMENT_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_SETTLEMENT_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentSettlement {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char settlement_reference[UMI_PAYMENTS_ID_CAPACITY];
    int64_t amount_minor;
    bool settled;
} UmiPaymentsPaymentSettlement;
UmiStatus umi_payments_payment_settlement_init(UmiPaymentsPaymentSettlement *value,
    const char *id,
    const char *payment_id,
    const char *settlement_reference,
    int64_t amount_minor,
    bool settled);
bool umi_payments_payment_settlement_valid(const UmiPaymentsPaymentSettlement *value);
bool umi_payments_payment_settlement_complete(const UmiPaymentsPaymentSettlement *value);
#ifdef __cplusplus
}
#endif
#endif
