/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_validation.h
 *
 * PURPOSE:
 *   Record payment control-gate evidence before release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_VALIDATION_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_VALIDATION_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment validation data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentValidation {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    uint32_t checks_passed;
    uint32_t checks_required;
    bool sanctions_clear;
    bool sufficient_funds;
} UmiPaymentsPaymentValidation;
/**
 * Initialise payments payment validation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_validation_init(UmiPaymentsPaymentValidation *value,
    const char *id,
    const char *payment_id,
    uint32_t checks_passed,
    uint32_t checks_required,
    bool sanctions_clear,
    bool sufficient_funds);
/**
 * Check that payments payment validation satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_validation_valid(const UmiPaymentsPaymentValidation *value);
/**
 * Provide the payments payment validation passed operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_validation_passed(const UmiPaymentsPaymentValidation *value);
#ifdef __cplusplus
}
#endif
#endif
