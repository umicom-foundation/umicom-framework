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

/**
 * Represent the workbench context event controller state data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context event controller state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_controller_state_init(
    UmiWorkbenchContextEventControllerState *record,
    const char *record_id);
/**
 * Check that workbench context event controller state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_controller_state_validate(
    const UmiWorkbenchContextEventControllerState *record);
/**
 * Provide the workbench context event controller state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_controller_state_set_source(
    UmiWorkbenchContextEventControllerState *record,
    const char *source_id);
/**
 * Provide the workbench context event controller state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_controller_state_set_subject(
    UmiWorkbenchContextEventControllerState *record,
    const char *subject_id);
/**
 * Provide the workbench context event controller state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_controller_state_set_group(
    UmiWorkbenchContextEventControllerState *record,
    const char *group_id);
/**
 * Provide the workbench context event controller state set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_controller_state_set_label(
    UmiWorkbenchContextEventControllerState *record,
    const char *label);
/**
 * Provide the workbench context event controller state hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_controller_state_hash(
    const UmiWorkbenchContextEventControllerState *record);
/**
 * Provide the workbench context event controller state touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_controller_state_touch(
    UmiWorkbenchContextEventControllerState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
