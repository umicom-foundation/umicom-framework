/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_return.h
 *
 * PURPOSE:
 *   Represent full or partial payment returns with bounded reason codes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_RETURN_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_RETURN_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentReturn {
    UmiFinancialId id;
    UmiFinancialId original_payment_id;
    char reason_code[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    int64_t amount_minor;
} UmiPaymentsPaymentReturn;
UmiStatus umi_payments_payment_return_init(UmiPaymentsPaymentReturn *value,
    const char *id,
    const char *original_payment_id,
    const char *reason_code,
    int64_t amount_minor);
bool umi_payments_payment_return_valid(const UmiPaymentsPaymentReturn *value);
#ifdef __cplusplus
}
#endif
#endif
