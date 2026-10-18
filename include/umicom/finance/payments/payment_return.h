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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_payment_return_archive_encode(const UmiPaymentsPaymentReturn *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_payment_return_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentReturn *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPaymentsPaymentReturn object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_payments_payment_return_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentReturn *workspace, UmiPaymentsPaymentReturn *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_payments_payment_return_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPaymentsPaymentReturn *expected, UmiPaymentsPaymentReturn *value);


#ifdef __cplusplus
}
#endif
#endif
