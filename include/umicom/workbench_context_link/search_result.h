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

void umi_workbench_context_link_search_result_init(UmiWorkbenchContextLinkSearchResult *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_search_result_validate(
    const UmiWorkbenchContextLinkSearchResult *record);
UmiStatus umi_workbench_context_link_search_result_copy(
    UmiWorkbenchContextLinkSearchResult *destination,
    const UmiWorkbenchContextLinkSearchResult *source);
uint64_t umi_workbench_context_link_search_result_hash(
    const UmiWorkbenchContextLinkSearchResult *record);
UmiStatus umi_workbench_context_link_search_result_set_primary(
    UmiWorkbenchContextLinkSearchResult *record,
    const char *value);
UmiStatus umi_workbench_context_link_search_result_set_secondary(
    UmiWorkbenchContextLinkSearchResult *record,
    const char *value);
void umi_workbench_context_link_search_result_touch(
    UmiWorkbenchContextLinkSearchResult *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
