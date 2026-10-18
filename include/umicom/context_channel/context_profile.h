/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_profile.h
 *
 * PURPOSE:
 *   Describe named context-channel profiles for Studio, Trader, TMS and Desk.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_PROFILE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_PROFILE_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context profile data shared with callers of this public contract.
 */
typedef struct UmiContextProfile {
    uint32_t structure_size;
    char profile_id[UMI_CONTEXT_VALUE_CAPACITY];
    char application_id[UMI_CONTEXT_VALUE_CAPACITY];
    char default_channel[UMI_CONTEXT_VALUE_CAPACITY];
    char default_schema[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextProfile;
/**
 * Initialise context profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_profile_init(UmiContextProfile *state);
/**
 * Provide the context profile set field operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_profile_set_field(UmiContextProfile *state,size_t field_index,const char *value);
/**
 * Provide the context profile field operation used by this module and its client
 * applications.
 */
const char *umi_context_profile_field(const UmiContextProfile *state,size_t field_index);
/**
 * Provide the context profile record success operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_profile_record_success(UmiContextProfile *state,uint64_t sequence);
/**
 * Provide the context profile record failure operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, failure_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_profile_record_failure(UmiContextProfile *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context profile satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_profile_validate(const UmiContextProfile *state);
/**
 * Provide the context profile covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_profile_covers_sequence(const UmiContextProfile *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_profile_archive_encode(const UmiContextProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiContextProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextProfile *expected, UmiContextProfile *value);


#ifdef __cplusplus
}
#endif
#endif
