/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_return.h
 *
 * PURPOSE:
 *   Represent full or partial payment returns with bounded reason codes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_RETURN_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_RETURN_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment return data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentReturn {
    UmiFinancialId id;
    UmiFinancialId original_payment_id;
    char reason_code[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    int64_t amount_minor;
} UmiPaymentsPaymentReturn;
/**
 * Initialise payments payment return from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_return_init(UmiPaymentsPaymentReturn *value,
    const char *id,
    const char *original_payment_id,
    const char *reason_code,
    int64_t amount_minor);
/**
 * Check that payments payment return satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_return_valid(const UmiPaymentsPaymentReturn *value);
#ifdef __cplusplus
}
#endif
#endif
