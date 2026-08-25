/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_batch.h
 *
 * PURPOSE:
 *   Represent approved same-currency payment batches using bounded aggregate evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_BATCH_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_BATCH_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentBatch {
    UmiFinancialId id;
    UmiCurrency currency;
    size_t payment_count;
    int64_t total_minor;
    bool approved;
} UmiPaymentsPaymentBatch;
UmiStatus umi_payments_payment_batch_init(UmiPaymentsPaymentBatch *value,
    const char *id,
    const char *currency_code,
    size_t payment_count,
    int64_t total_minor,
    bool approved);
bool umi_payments_payment_batch_valid(const UmiPaymentsPaymentBatch *value);
bool umi_payments_payment_batch_releasable(const UmiPaymentsPaymentBatch *value);
#ifdef __cplusplus
}
#endif
#endif
