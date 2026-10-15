/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_event.h
 *
 * PURPOSE:
 *   Record sequence-ordered treasury domain events with event timestamp.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_EVENT_H
#define UMICOM_FINANCE_TREASURY_TREASURY_EVENT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury treasury event data shared with callers of this public contract.
 */
typedef struct UmiTreasuryTreasuryEvent {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint64_t sequence;
    int64_t event_epoch_millis;
} UmiTreasuryTreasuryEvent;
/**
 * Initialise treasury treasury event from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_event_init(UmiTreasuryTreasuryEvent *value,
    const char *id,
    uint64_t sequence,
    int64_t event_epoch_millis);
/**
 * Check that treasury treasury event satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_event_valid(const UmiTreasuryTreasuryEvent *value);
/**
 * Provide the treasury treasury event event sequence operation used by this module and its
 * client applications.
 */
uint64_t umi_treasury_treasury_event_event_sequence(const UmiTreasuryTreasuryEvent *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_treasury_event_archive_encode(const UmiTreasuryTreasuryEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_treasury_event_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryTreasuryEvent *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_treasury_event_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryTreasuryEvent *expected, UmiTreasuryTreasuryEvent *value);


#ifdef __cplusplus
}
#endif
#endif
