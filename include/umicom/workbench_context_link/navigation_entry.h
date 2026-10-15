/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/navigation_entry.h
 *
 * PURPOSE:
 *   Define the reusable context navigation stack entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_NAVIGATION_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_NAVIGATION_ENTRY_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link navigation entry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkNavigationEntry {
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
} UmiWorkbenchContextLinkNavigationEntry;

/**
 * Initialise workbench context link navigation entry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_navigation_entry_init(UmiWorkbenchContextLinkNavigationEntry *record,
                                           const char *identity);
/**
 * Check that workbench context link navigation entry satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_navigation_entry_validate(
    const UmiWorkbenchContextLinkNavigationEntry *record);
/**
 * Copy workbench context link navigation entry into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_navigation_entry_copy(
    UmiWorkbenchContextLinkNavigationEntry *destination,
    const UmiWorkbenchContextLinkNavigationEntry *source);
/**
 * Provide the workbench context link navigation entry hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_navigation_entry_hash(
    const UmiWorkbenchContextLinkNavigationEntry *record);
/**
 * Provide the workbench context link navigation entry set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_navigation_entry_set_primary(
    UmiWorkbenchContextLinkNavigationEntry *record,
    const char *value);
/**
 * Provide the workbench context link navigation entry set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_navigation_entry_set_secondary(
    UmiWorkbenchContextLinkNavigationEntry *record,
    const char *value);
/**
 * Provide the workbench context link navigation entry touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_navigation_entry_touch(
    UmiWorkbenchContextLinkNavigationEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_navigation_entry_archive_encode(const UmiWorkbenchContextLinkNavigationEntry *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_navigation_entry_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkNavigationEntry *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_context_link_navigation_entry_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchContextLinkNavigationEntry *expected, UmiWorkbenchContextLinkNavigationEntry *value);


/** Replace this inline value only while the observed revision is current.
 * Keep the same identity and a valid domain value. On failure nothing changes;
 * successful publication advances the live revision once. The proposal token
 * does not grant authority. The caller serializes access to the owner. */
UmiStatus umi_workbench_context_link_navigation_entry_replace_if_current(UmiWorkbenchContextLinkNavigationEntry *value,
    uint64_t expected_revision, const UmiWorkbenchContextLinkNavigationEntry *proposal);

#ifdef __cplusplus
}
#endif

#endif
