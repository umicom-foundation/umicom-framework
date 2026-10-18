/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_backpressure.h
 *
 * PURPOSE:
 *   Expose subscriber backlog and delivery pressure for operational control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BACKPRESSURE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BACKPRESSURE_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context backpressure data shared with callers of this public contract.
 */
typedef struct UmiContextBackpressure {
    uint32_t structure_size;
    char pressure_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subscription_id[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBackpressure;
/**
 * Initialise context backpressure from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_backpressure_init(UmiContextBackpressure *state);
/**
 * Provide the context backpressure set field operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_backpressure_set_field(UmiContextBackpressure *state,size_t field_index,const char *value);
/**
 * Provide the context backpressure field operation used by this module and its client
 * applications.
 */
const char *umi_context_backpressure_field(const UmiContextBackpressure *state,size_t field_index);
/**
 * Provide the context backpressure record success operation used by this module and its
 * client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_backpressure_record_success(UmiContextBackpressure *state,uint64_t sequence);
/**
 * Provide the context backpressure record failure operation used by this module and its
 * client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, failure_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_backpressure_record_failure(UmiContextBackpressure *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context backpressure satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_backpressure_validate(const UmiContextBackpressure *state);
/**
 * Provide the context backpressure covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_backpressure_covers_sequence(const UmiContextBackpressure *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_backpressure_archive_encode(const UmiContextBackpressure *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_backpressure_archive_decode(const void *bytes, size_t byte_count,
    UmiContextBackpressure *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiContextBackpressure object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_backpressure_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiContextBackpressure *workspace, UmiContextBackpressure *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_backpressure_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextBackpressure *expected, UmiContextBackpressure *value);


#ifdef __cplusplus
}
#endif
#endif
