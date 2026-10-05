/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/filter.h
 *
 * PURPOSE:
 *   Define the reusable context-link filter contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_FILTER_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_FILTER_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link filter data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkFilter {
    uint32_t structure_size;
    char filter_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char query_text[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkFilter;

/**
 * Initialise workbench context link filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_link_filter_init(UmiWorkbenchContextLinkFilter *record,
                                           const char *identity);
/**
 * Check that workbench context link filter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_filter_validate(
    const UmiWorkbenchContextLinkFilter *record);
/**
 * Copy workbench context link filter into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_context_link_filter_copy(
    UmiWorkbenchContextLinkFilter *destination,
    const UmiWorkbenchContextLinkFilter *source);
/**
 * Provide the workbench context link filter hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_filter_hash(
    const UmiWorkbenchContextLinkFilter *record);
/**
 * Provide the workbench context link filter set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_filter_set_primary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value);
/**
 * Provide the workbench context link filter set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_filter_set_secondary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value);
/**
 * Provide the workbench context link filter touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_filter_touch(
    UmiWorkbenchContextLinkFilter *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_filter_archive_encode(const UmiWorkbenchContextLinkFilter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_filter_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkFilter *value);

#ifdef __cplusplus
}
#endif

#endif
