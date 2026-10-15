/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/history_entry.h
 *
 * PURPOSE:
 *   Define the reusable context navigation history entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_ENTRY_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link history entry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkHistoryEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkHistoryEntry;

/**
 * Initialise workbench context link history entry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_history_entry_init(UmiWorkbenchContextLinkHistoryEntry *record,
                                           const char *identity);
/**
 * Check that workbench context link history entry satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_history_entry_validate(
    const UmiWorkbenchContextLinkHistoryEntry *record);
/**
 * Copy workbench context link history entry into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_history_entry_copy(
    UmiWorkbenchContextLinkHistoryEntry *destination,
    const UmiWorkbenchContextLinkHistoryEntry *source);
/**
 * Provide the workbench context link history entry hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_history_entry_hash(
    const UmiWorkbenchContextLinkHistoryEntry *record);
/**
 * Provide the workbench context link history entry set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_history_entry_set_primary(
    UmiWorkbenchContextLinkHistoryEntry *record,
    const char *value);
/**
 * Provide the workbench context link history entry set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_history_entry_set_secondary(
    UmiWorkbenchContextLinkHistoryEntry *record,
    const char *value);
/**
 * Provide the workbench context link history entry touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_history_entry_touch(
    UmiWorkbenchContextLinkHistoryEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_history_entry_archive_encode(const UmiWorkbenchContextLinkHistoryEntry *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_history_entry_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkHistoryEntry *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_context_link_history_entry_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchContextLinkHistoryEntry *expected, UmiWorkbenchContextLinkHistoryEntry *value);


/** Replace this inline value only while the observed revision is current.
 * Keep the same identity and a valid domain value. On failure nothing changes;
 * successful publication advances the live revision once. The proposal token
 * does not grant authority. The caller serializes access to the owner. */
UmiStatus umi_workbench_context_link_history_entry_replace_if_current(UmiWorkbenchContextLinkHistoryEntry *value,
    uint64_t expected_revision, const UmiWorkbenchContextLinkHistoryEntry *proposal);

#ifdef __cplusplus
}
#endif

#endif
