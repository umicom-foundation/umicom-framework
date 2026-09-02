/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/queue_state.h
 *
 * PURPOSE:
 *   Define the reusable event queue state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_QUEUE_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_QUEUE_STATE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event queue state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventQueueState {
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
} UmiWorkbenchContextEventQueueState;

/**
 * Initialise workbench context event queue state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_queue_state_init(
    UmiWorkbenchContextEventQueueState *record,
    const char *record_id);
/**
 * Check that workbench context event queue state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_queue_state_validate(
    const UmiWorkbenchContextEventQueueState *record);
/**
 * Provide the workbench context event queue state set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_queue_state_set_source(
    UmiWorkbenchContextEventQueueState *record,
    const char *source_id);
/**
 * Provide the workbench context event queue state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_queue_state_set_subject(
    UmiWorkbenchContextEventQueueState *record,
    const char *subject_id);
/**
 * Provide the workbench context event queue state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_queue_state_set_group(
    UmiWorkbenchContextEventQueueState *record,
    const char *group_id);
/**
 * Provide the workbench context event queue state set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_queue_state_set_label(
    UmiWorkbenchContextEventQueueState *record,
    const char *label);
/**
 * Provide the workbench context event queue state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_queue_state_hash(
    const UmiWorkbenchContextEventQueueState *record);
/**
 * Provide the workbench context event queue state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_queue_state_touch(
    UmiWorkbenchContextEventQueueState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
