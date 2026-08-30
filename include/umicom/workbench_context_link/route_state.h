/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/route_state.h
 *
 * PURPOSE:
 *   Define the reusable route runtime state record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkRouteState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char route_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkRouteState;

void umi_workbench_context_link_route_state_init(UmiWorkbenchContextLinkRouteState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_route_state_validate(
    const UmiWorkbenchContextLinkRouteState *record);
UmiStatus umi_workbench_context_link_route_state_copy(
    UmiWorkbenchContextLinkRouteState *destination,
    const UmiWorkbenchContextLinkRouteState *source);
uint64_t umi_workbench_context_link_route_state_hash(
    const UmiWorkbenchContextLinkRouteState *record);
UmiStatus umi_workbench_context_link_route_state_set_primary(
    UmiWorkbenchContextLinkRouteState *record,
    const char *value);
UmiStatus umi_workbench_context_link_route_state_set_secondary(
    UmiWorkbenchContextLinkRouteState *record,
    const char *value);
void umi_workbench_context_link_route_state_touch(
    UmiWorkbenchContextLinkRouteState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
