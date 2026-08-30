/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/route_hop.h
 *
 * PURPOSE:
 *   Define the reusable context route hop contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_HOP_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_HOP_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkRouteHop {
    uint32_t structure_size;
    char hop_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char destination_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkRouteHop;

void umi_workbench_context_link_route_hop_init(UmiWorkbenchContextLinkRouteHop *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_route_hop_validate(
    const UmiWorkbenchContextLinkRouteHop *record);
UmiStatus umi_workbench_context_link_route_hop_copy(
    UmiWorkbenchContextLinkRouteHop *destination,
    const UmiWorkbenchContextLinkRouteHop *source);
uint64_t umi_workbench_context_link_route_hop_hash(
    const UmiWorkbenchContextLinkRouteHop *record);
UmiStatus umi_workbench_context_link_route_hop_set_primary(
    UmiWorkbenchContextLinkRouteHop *record,
    const char *value);
UmiStatus umi_workbench_context_link_route_hop_set_secondary(
    UmiWorkbenchContextLinkRouteHop *record,
    const char *value);
void umi_workbench_context_link_route_hop_touch(
    UmiWorkbenchContextLinkRouteHop *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
