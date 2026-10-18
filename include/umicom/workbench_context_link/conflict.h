/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/conflict.h
 *
 * PURPOSE:
 *   Define the reusable context-link conflict record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONFLICT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONFLICT_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link conflict data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkConflict {
    uint32_t structure_size;
    char conflict_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkConflict;

/**
 * Initialise workbench context link conflict from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_conflict_init(UmiWorkbenchContextLinkConflict *record,
                                           const char *identity);
/**
 * Check that workbench context link conflict satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_conflict_validate(
    const UmiWorkbenchContextLinkConflict *record);
/**
 * Copy workbench context link conflict into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_context_link_conflict_copy(
    UmiWorkbenchContextLinkConflict *destination,
    const UmiWorkbenchContextLinkConflict *source);
/**
 * Provide the workbench context link conflict hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_conflict_hash(
    const UmiWorkbenchContextLinkConflict *record);
/**
 * Provide the workbench context link conflict set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_conflict_set_primary(
    UmiWorkbenchContextLinkConflict *record,
    const char *value);
/**
 * Provide the workbench context link conflict set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_conflict_set_secondary(
    UmiWorkbenchContextLinkConflict *record,
    const char *value);
/**
 * Provide the workbench context link conflict touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_conflict_touch(
    UmiWorkbenchContextLinkConflict *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_conflict_archive_encode(const UmiWorkbenchContextLinkConflict *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_conflict_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkConflict *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiWorkbenchContextLinkConflict object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_workbench_context_link_conflict_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkConflict *workspace, UmiWorkbenchContextLinkConflict *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_context_link_conflict_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchContextLinkConflict *expected, UmiWorkbenchContextLinkConflict *value);


/** Replace this inline value only while the observed revision is current.
 * Keep the same identity and a valid domain value. On failure nothing changes;
 * successful publication advances the live revision once. The proposal token
 * does not grant authority. The caller serializes access to the owner. */
UmiStatus umi_workbench_context_link_conflict_replace_if_current(UmiWorkbenchContextLinkConflict *value,
    uint64_t expected_revision, const UmiWorkbenchContextLinkConflict *proposal);

#ifdef __cplusplus
}
#endif

#endif
