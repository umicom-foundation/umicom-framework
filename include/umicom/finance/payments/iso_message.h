/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/iso_message.h
 *
 * PURPOSE:
 *   Represent ISO-20022-style business identifiers without binding to an XML parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_ISO_MESSAGE_H
#define UMICOM_FINANCE_PAYMENTS_ISO_MESSAGE_H
#include "umicom/finance/payments/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments iso message data shared with callers of this public contract.
 */
typedef struct UmiPaymentsIsoMessage {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char message_family[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    char end_to_end_id[UMI_PAYMENTS_ID_CAPACITY];
} UmiPaymentsIsoMessage;
/**
 * Initialise payments iso message from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_payments_iso_message_init(UmiPaymentsIsoMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_family,
    const char *end_to_end_id);
/**
 * Check that payments iso message satisfies its contract before another service relies on
 * it.
 */
bool umi_payments_iso_message_valid(const UmiPaymentsIsoMessage *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_iso_message_archive_encode(const UmiPaymentsIsoMessage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_iso_message_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsIsoMessage *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPaymentsIsoMessage object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_payments_iso_message_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPaymentsIsoMessage *workspace, UmiPaymentsIsoMessage *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_payments_iso_message_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPaymentsIsoMessage *expected, UmiPaymentsIsoMessage *value);


#ifdef __cplusplus
}
#endif
#endif
