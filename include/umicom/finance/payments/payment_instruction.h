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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_payment_instruction_archive_encode(const UmiPaymentsPaymentInstruction *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_payment_instruction_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentInstruction *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPaymentsPaymentInstruction object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_payments_payment_instruction_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentInstruction *workspace, UmiPaymentsPaymentInstruction *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_payments_payment_instruction_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPaymentsPaymentInstruction *expected, UmiPaymentsPaymentInstruction *value);


#ifdef __cplusplus
}
#endif
#endif
