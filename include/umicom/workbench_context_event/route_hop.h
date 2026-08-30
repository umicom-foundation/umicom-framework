/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/route_hop.h
 *
 * PURPOSE:
 *   Define the reusable event route hop contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_ROUTE_HOP_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_ROUTE_HOP_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventRouteHop {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventRouteHop;

void umi_workbench_context_event_route_hop_init(
    UmiWorkbenchContextEventRouteHop *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_route_hop_validate(
    const UmiWorkbenchContextEventRouteHop *record);
UmiStatus umi_workbench_context_event_route_hop_set_source(
    UmiWorkbenchContextEventRouteHop *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_route_hop_set_subject(
    UmiWorkbenchContextEventRouteHop *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_route_hop_set_group(
    UmiWorkbenchContextEventRouteHop *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_route_hop_set_label(
    UmiWorkbenchContextEventRouteHop *record,
    const char *label);
uint64_t umi_workbench_context_event_route_hop_hash(
    const UmiWorkbenchContextEventRouteHop *record);
void umi_workbench_context_event_route_hop_touch(
    UmiWorkbenchContextEventRouteHop *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
