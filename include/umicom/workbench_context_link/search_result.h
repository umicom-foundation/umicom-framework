/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/search_result.h
 *
 * PURPOSE:
 *   Define the reusable context-link search result contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SEARCH_RESULT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SEARCH_RESULT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link search result data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkSearchResult {
    uint32_t structure_size;
    char result_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkSearchResult;

/**
 * Initialise workbench context link search result from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_search_result_init(UmiWorkbenchContextLinkSearchResult *record,
                                           const char *identity);
/**
 * Check that workbench context link search result satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_search_result_validate(
    const UmiWorkbenchContextLinkSearchResult *record);
/**
 * Copy workbench context link search result into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_search_result_copy(
    UmiWorkbenchContextLinkSearchResult *destination,
    const UmiWorkbenchContextLinkSearchResult *source);
/**
 * Provide the workbench context link search result hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_search_result_hash(
    const UmiWorkbenchContextLinkSearchResult *record);
/**
 * Provide the workbench context link search result set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_search_result_set_primary(
    UmiWorkbenchContextLinkSearchResult *record,
    const char *value);
/**
 * Provide the workbench context link search result set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_search_result_set_secondary(
    UmiWorkbenchContextLinkSearchResult *record,
    const char *value);
/**
 * Provide the workbench context link search result touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_search_result_touch(
    UmiWorkbenchContextLinkSearchResult *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
