/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/controller_state.h
 *
 * PURPOSE:
 *   Define the reusable event controller state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_CONTROLLER_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_CONTROLLER_STATE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventControllerState {
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
} UmiWorkbenchContextEventControllerState;

void umi_workbench_context_event_controller_state_init(
    UmiWorkbenchContextEventControllerState *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_controller_state_validate(
    const UmiWorkbenchContextEventControllerState *record);
UmiStatus umi_workbench_context_event_controller_state_set_source(
    UmiWorkbenchContextEventControllerState *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_controller_state_set_subject(
    UmiWorkbenchContextEventControllerState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_controller_state_set_group(
    UmiWorkbenchContextEventControllerState *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_controller_state_set_label(
    UmiWorkbenchContextEventControllerState *record,
    const char *label);
uint64_t umi_workbench_context_event_controller_state_hash(
    const UmiWorkbenchContextEventControllerState *record);
void umi_workbench_context_event_controller_state_touch(
    UmiWorkbenchContextEventControllerState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
