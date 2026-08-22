/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/route.h
 *
 * PURPOSE:
 *   Define the reusable event routing rule contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_ROUTE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_ROUTE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventRoute {
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
} UmiWorkbenchContextEventRoute;

void umi_workbench_context_event_route_init(
    UmiWorkbenchContextEventRoute *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_route_validate(
    const UmiWorkbenchContextEventRoute *record);
UmiStatus umi_workbench_context_event_route_set_source(
    UmiWorkbenchContextEventRoute *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_route_set_subject(
    UmiWorkbenchContextEventRoute *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_route_set_group(
    UmiWorkbenchContextEventRoute *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_route_set_label(
    UmiWorkbenchContextEventRoute *record,
    const char *label);
uint64_t umi_workbench_context_event_route_hash(
    const UmiWorkbenchContextEventRoute *record);
void umi_workbench_context_event_route_touch(
    UmiWorkbenchContextEventRoute *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
