/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/route_trace.h
 *
 * PURPOSE:
 *   Define the reusable context route trace contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_TRACE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_TRACE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkRouteTrace {
    uint32_t structure_size;
    char trace_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkRouteTrace;

void umi_workbench_context_link_route_trace_init(UmiWorkbenchContextLinkRouteTrace *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_route_trace_validate(
    const UmiWorkbenchContextLinkRouteTrace *record);
UmiStatus umi_workbench_context_link_route_trace_copy(
    UmiWorkbenchContextLinkRouteTrace *destination,
    const UmiWorkbenchContextLinkRouteTrace *source);
uint64_t umi_workbench_context_link_route_trace_hash(
    const UmiWorkbenchContextLinkRouteTrace *record);
UmiStatus umi_workbench_context_link_route_trace_set_primary(
    UmiWorkbenchContextLinkRouteTrace *record,
    const char *value);
UmiStatus umi_workbench_context_link_route_trace_set_secondary(
    UmiWorkbenchContextLinkRouteTrace *record,
    const char *value);
void umi_workbench_context_link_route_trace_touch(
    UmiWorkbenchContextLinkRouteTrace *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
