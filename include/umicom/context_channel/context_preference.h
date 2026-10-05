/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_preference.h
 *
 * PURPOSE:
 *   Store user-facing context-link preferences independently of GTK widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_PREFERENCE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_PREFERENCE_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context preference data shared with callers of this public contract.
 */
typedef struct UmiContextPreference {
    uint32_t structure_size;
    char preference_id[UMI_CONTEXT_VALUE_CAPACITY];
    char user_id[UMI_CONTEXT_VALUE_CAPACITY];
    char default_colour[UMI_CONTEXT_VALUE_CAPACITY];
    char default_channel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextPreference;
/**
 * Initialise context preference from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_preference_init(UmiContextPreference *state);
/**
 * Provide the context preference set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_preference_set_field(UmiContextPreference *state,size_t field_index,const char *value);
/**
 * Provide the context preference field operation used by this module and its client
 * applications.
 */
const char *umi_context_preference_field(const UmiContextPreference *state,size_t field_index);
/**
 * Provide the context preference record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_preference_record_success(UmiContextPreference *state,uint64_t sequence);
/**
 * Provide the context preference record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_preference_record_failure(UmiContextPreference *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context preference satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_preference_validate(const UmiContextPreference *state);
/**
 * Provide the context preference covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_preference_covers_sequence(const UmiContextPreference *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_preference_archive_encode(const UmiContextPreference *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_preference_archive_decode(const void *bytes, size_t byte_count,
    UmiContextPreference *value);

#ifdef __cplusplus
}
#endif
#endif
