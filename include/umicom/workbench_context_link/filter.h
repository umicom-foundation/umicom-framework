/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/filter.h
 *
 * PURPOSE:
 *   Define the reusable context-link filter contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_FILTER_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_FILTER_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_filter_init(UmiWorkbenchContextLinkFilter *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_filter_validate(
    const UmiWorkbenchContextLinkFilter *record);
UmiStatus umi_workbench_context_link_filter_copy(
    UmiWorkbenchContextLinkFilter *destination,
    const UmiWorkbenchContextLinkFilter *source);
uint64_t umi_workbench_context_link_filter_hash(
    const UmiWorkbenchContextLinkFilter *record);
UmiStatus umi_workbench_context_link_filter_set_primary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value);
UmiStatus umi_workbench_context_link_filter_set_secondary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value);
void umi_workbench_context_link_filter_touch(
    UmiWorkbenchContextLinkFilter *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
