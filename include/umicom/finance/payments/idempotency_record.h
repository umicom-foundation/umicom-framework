/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/idempotency_record.h
 *
 * PURPOSE:
 *   Bind an idempotency key and request fingerprint to one canonical payment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_IDEMPOTENCY_RECORD_H
#define UMICOM_FINANCE_PAYMENTS_IDEMPOTENCY_RECORD_H
#include "umicom/finance/payments/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments idempotency record data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsIdempotencyRecord {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char idempotency_key[UMI_PAYMENTS_ID_CAPACITY];
    uint64_t fingerprint;
} UmiPaymentsIdempotencyRecord;
/**
 * Initialise payments idempotency record from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_idempotency_record_init(UmiPaymentsIdempotencyRecord *value,
    const char *id,
    const char *payment_id,
    const char *idempotency_key,
    uint64_t fingerprint);
/**
 * Check that payments idempotency record satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_idempotency_record_valid(const UmiPaymentsIdempotencyRecord *value);
/**
 * Provide the payments idempotency record replay safe operation used by this module and
 * its client applications.
 */
bool umi_payments_idempotency_record_replay_safe(const UmiPaymentsIdempotencyRecord *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_idempotency_record_archive_encode(const UmiPaymentsIdempotencyRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_idempotency_record_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsIdempotencyRecord *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_payments_idempotency_record_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPaymentsIdempotencyRecord *expected, UmiPaymentsIdempotencyRecord *value);


#ifdef __cplusplus
}
#endif
#endif
