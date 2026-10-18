/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_message.h
 *
 * PURPOSE:
 *   Represent durable canonical payment message metadata and direction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_MESSAGE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_MESSAGE_H
#include "umicom/finance/payments/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment message data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentMessage {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char message_type[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    UmiPaymentsMessageDirection direction;
    uint64_t sequence;
} UmiPaymentsPaymentMessage;
/**
 * Initialise payments payment message from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_message_init(UmiPaymentsPaymentMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_type,
    UmiPaymentsMessageDirection direction,
    uint64_t sequence);
/**
 * Check that payments payment message satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_message_valid(const UmiPaymentsPaymentMessage *value);
/**
 * Provide the payments payment message outbound operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_message_outbound(const UmiPaymentsPaymentMessage *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_payment_message_archive_encode(const UmiPaymentsPaymentMessage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_payment_message_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentMessage *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPaymentsPaymentMessage object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_payments_payment_message_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentMessage *workspace, UmiPaymentsPaymentMessage *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_payments_payment_message_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPaymentsPaymentMessage *expected, UmiPaymentsPaymentMessage *value);


#ifdef __cplusplus
}
#endif
#endif
