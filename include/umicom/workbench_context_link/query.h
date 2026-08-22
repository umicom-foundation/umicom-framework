/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/query.h
 *
 * PURPOSE:
 *   Define the reusable context-link query record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_QUERY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_QUERY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkQuery {
    uint32_t structure_size;
    char query_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkQuery;

void umi_workbench_context_link_query_init(UmiWorkbenchContextLinkQuery *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_query_validate(
    const UmiWorkbenchContextLinkQuery *record);
UmiStatus umi_workbench_context_link_query_copy(
    UmiWorkbenchContextLinkQuery *destination,
    const UmiWorkbenchContextLinkQuery *source);
uint64_t umi_workbench_context_link_query_hash(
    const UmiWorkbenchContextLinkQuery *record);
UmiStatus umi_workbench_context_link_query_set_primary(
    UmiWorkbenchContextLinkQuery *record,
    const char *value);
UmiStatus umi_workbench_context_link_query_set_secondary(
    UmiWorkbenchContextLinkQuery *record,
    const char *value);
void umi_workbench_context_link_query_touch(
    UmiWorkbenchContextLinkQuery *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
