/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_instruction.h
 *
 * PURPOSE:
 *   Represent canonical payment economic meaning, parties, amount and lifecycle state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_INSTRUCTION_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_INSTRUCTION_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
UmiStatus umi_payments_payment_instruction_init(UmiPaymentsPaymentInstruction *value,
    const char *id,
    const char *debtor_party_id,
    const char *creditor_party_id,
    const char *currency_code,
    int64_t amount_minor,
    UmiFinancialDate requested_date,
    UmiPaymentsStatus status,
    const char *idempotency_key);
bool umi_payments_payment_instruction_valid(const UmiPaymentsPaymentInstruction *value);
bool umi_payments_payment_instruction_final(const UmiPaymentsPaymentInstruction *value);
#ifdef __cplusplus
}
#endif
#endif
