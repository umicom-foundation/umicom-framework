/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_clearing.h
 *
 * PURPOSE:
 *   Represent gross/net clearing values and participant count for payment rails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_CLEARING_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_CLEARING_H
#include "umicom/finance/payments/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment clearing data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentClearing {
    UmiFinancialId id;
    int64_t gross_minor;
    int64_t net_minor;
    size_t participant_count;
    bool complete;
} UmiPaymentsPaymentClearing;
/**
 * Initialise payments payment clearing from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_clearing_init(UmiPaymentsPaymentClearing *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor,
    size_t participant_count,
    bool complete);
/**
 * Check that payments payment clearing satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_clearing_valid(const UmiPaymentsPaymentClearing *value);
/**
 * Provide the payments payment clearing cleared operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_clearing_cleared(const UmiPaymentsPaymentClearing *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_payment_clearing_archive_encode(const UmiPaymentsPaymentClearing *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_payment_clearing_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentClearing *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_payments_payment_clearing_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPaymentsPaymentClearing *expected, UmiPaymentsPaymentClearing *value);


#ifdef __cplusplus
}
#endif
#endif
