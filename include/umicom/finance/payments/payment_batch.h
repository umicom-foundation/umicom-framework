/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_batch.h
 *
 * PURPOSE:
 *   Represent approved same-currency payment batches using bounded aggregate evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_BATCH_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_BATCH_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment batch data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentBatch {
    UmiFinancialId id;
    UmiCurrency currency;
    size_t payment_count;
    int64_t total_minor;
    bool approved;
} UmiPaymentsPaymentBatch;
/**
 * Initialise payments payment batch from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_batch_init(UmiPaymentsPaymentBatch *value,
    const char *id,
    const char *currency_code,
    size_t payment_count,
    int64_t total_minor,
    bool approved);
/**
 * Check that payments payment batch satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_batch_valid(const UmiPaymentsPaymentBatch *value);
/**
 * Provide the payments payment batch releasable operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_batch_releasable(const UmiPaymentsPaymentBatch *value);
#ifdef __cplusplus
}
#endif
#endif
