/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/endpoint_binding.h
 *
 * PURPOSE:
 *   Define the reusable endpoint event binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_ENDPOINT_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_ENDPOINT_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventEndpointBinding {
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
} UmiWorkbenchContextEventEndpointBinding;

void umi_workbench_context_event_endpoint_binding_init(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_endpoint_binding_validate(
    const UmiWorkbenchContextEventEndpointBinding *record);
UmiStatus umi_workbench_context_event_endpoint_binding_set_source(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_endpoint_binding_set_subject(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_endpoint_binding_set_group(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_endpoint_binding_set_label(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *label);
uint64_t umi_workbench_context_event_endpoint_binding_hash(
    const UmiWorkbenchContextEventEndpointBinding *record);
void umi_workbench_context_event_endpoint_binding_touch(
    UmiWorkbenchContextEventEndpointBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
