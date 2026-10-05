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

#ifdef __cplusplus
}
#endif

#endif
