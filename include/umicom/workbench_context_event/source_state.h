/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/source_state.h
 *
 * PURPOSE:
 *   Define the reusable event source state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SOURCE_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SOURCE_STATE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventSourceState {
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
} UmiWorkbenchContextEventSourceState;

void umi_workbench_context_event_source_state_init(
    UmiWorkbenchContextEventSourceState *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_source_state_validate(
    const UmiWorkbenchContextEventSourceState *record);
UmiStatus umi_workbench_context_event_source_state_set_source(
    UmiWorkbenchContextEventSourceState *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_source_state_set_subject(
    UmiWorkbenchContextEventSourceState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_source_state_set_group(
    UmiWorkbenchContextEventSourceState *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_source_state_set_label(
    UmiWorkbenchContextEventSourceState *record,
    const char *label);
uint64_t umi_workbench_context_event_source_state_hash(
    const UmiWorkbenchContextEventSourceState *record);
void umi_workbench_context_event_source_state_touch(
    UmiWorkbenchContextEventSourceState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
