/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/statistics.h
 *
 * PURPOSE:
 *   Define the reusable context-link statistics record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_STATISTICS_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_STATISTICS_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link statistics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkStatistics {
    uint32_t structure_size;
    char statistics_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char scope_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkStatistics;

/**
 * Initialise workbench context link statistics from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_statistics_init(UmiWorkbenchContextLinkStatistics *record,
                                           const char *identity);
/**
 * Check that workbench context link statistics satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_statistics_validate(
    const UmiWorkbenchContextLinkStatistics *record);
/**
 * Copy workbench context link statistics into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_statistics_copy(
    UmiWorkbenchContextLinkStatistics *destination,
    const UmiWorkbenchContextLinkStatistics *source);
/**
 * Provide the workbench context link statistics hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_statistics_hash(
    const UmiWorkbenchContextLinkStatistics *record);
/**
 * Provide the workbench context link statistics set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_statistics_set_primary(
    UmiWorkbenchContextLinkStatistics *record,
    const char *value);
/**
 * Provide the workbench context link statistics set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_statistics_set_secondary(
    UmiWorkbenchContextLinkStatistics *record,
    const char *value);
/**
 * Provide the workbench context link statistics touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_statistics_touch(
    UmiWorkbenchContextLinkStatistics *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_statistics_archive_encode(const UmiWorkbenchContextLinkStatistics *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_statistics_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkStatistics *value);

#ifdef __cplusplus
}
#endif

#endif
