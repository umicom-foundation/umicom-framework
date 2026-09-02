/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_instruction.h
 *
 * PURPOSE:
 *   Represent canonical payment economic meaning, parties, amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_INSTRUCTION_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_INSTRUCTION_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment instruction data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentInstruction {
    UmiFinancialId id;
    UmiFinancialId debtor_party_id;
    UmiFinancialId creditor_party_id;
    UmiCurrency currency;
    int64_t amount_minor;
    UmiFinancialDate requested_date;
    UmiPaymentsStatus status;
    char idempotency_key[UMI_PAYMENTS_ID_CAPACITY];
} UmiPaymentsPaymentInstruction;
/**
 * Initialise payments payment instruction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_instruction_init(UmiPaymentsPaymentInstruction *value,
    const char *id,
    const char *debtor_party_id,
    const char *creditor_party_id,
    const char *currency_code,
    int64_t amount_minor,
    UmiFinancialDate requested_date,
    UmiPaymentsStatus status,
    const char *idempotency_key);
/**
 * Check that payments payment instruction satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_instruction_valid(const UmiPaymentsPaymentInstruction *value);
/**
 * Provide the payments payment instruction final operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_instruction_final(const UmiPaymentsPaymentInstruction *value);
#ifdef __cplusplus
}
#endif
#endif
