/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_browser.h
 *
 * PURPOSE:
 *   Build immutable browser rows for recent, pinned and active contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BROWSER_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BROWSER_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context browser data shared with callers of this public contract.
 */
typedef struct UmiContextBrowser {
    uint32_t structure_size;
    char filter_text[UMI_CONTEXT_VALUE_CAPACITY];
    char category[UMI_CONTEXT_VALUE_CAPACITY];
    char owner_id[UMI_CONTEXT_VALUE_CAPACITY];
    char workspace_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBrowser;
/**
 * Initialise context browser from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_browser_init(UmiContextBrowser *state);
/**
 * Provide the context browser set field operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_browser_set_field(UmiContextBrowser *state,size_t field_index,const char *value);
/**
 * Provide the context browser field operation used by this module and its client
 * applications.
 */
const char *umi_context_browser_field(const UmiContextBrowser *state,size_t field_index);
/**
 * Provide the context browser record success operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_browser_record_success(UmiContextBrowser *state,uint64_t sequence);
/**
 * Provide the context browser record failure operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, failure_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_browser_record_failure(UmiContextBrowser *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context browser satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_browser_validate(const UmiContextBrowser *state);
/**
 * Provide the context browser covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_browser_covers_sequence(const UmiContextBrowser *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_browser_archive_encode(const UmiContextBrowser *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_browser_archive_decode(const void *bytes, size_t byte_count,
    UmiContextBrowser *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiContextBrowser object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_browser_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiContextBrowser *workspace, UmiContextBrowser *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_browser_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextBrowser *expected, UmiContextBrowser *value);


#ifdef __cplusplus
}
#endif
#endif
