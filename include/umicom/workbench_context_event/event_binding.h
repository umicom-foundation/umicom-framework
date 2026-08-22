/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/event_binding.h
 *
 * PURPOSE:
 *   Define the reusable event-to-context binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventEventBinding {
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
} UmiWorkbenchContextEventEventBinding;

void umi_workbench_context_event_event_binding_init(
    UmiWorkbenchContextEventEventBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_event_binding_validate(
    const UmiWorkbenchContextEventEventBinding *record);
UmiStatus umi_workbench_context_event_event_binding_set_source(
    UmiWorkbenchContextEventEventBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_event_binding_set_subject(
    UmiWorkbenchContextEventEventBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_event_binding_set_group(
    UmiWorkbenchContextEventEventBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_event_binding_set_label(
    UmiWorkbenchContextEventEventBinding *record,
    const char *label);
uint64_t umi_workbench_context_event_event_binding_hash(
    const UmiWorkbenchContextEventEventBinding *record);
void umi_workbench_context_event_event_binding_touch(
    UmiWorkbenchContextEventEventBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
