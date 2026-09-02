/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/replay_request.h
 *
 * PURPOSE:
 *   Define the reusable event replay request contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_REPLAY_REQUEST_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_REPLAY_REQUEST_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event replay request data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventReplayRequest {
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
} UmiWorkbenchContextEventReplayRequest;

/**
 * Initialise workbench context event replay request from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_replay_request_init(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *record_id);
/**
 * Check that workbench context event replay request satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_replay_request_validate(
    const UmiWorkbenchContextEventReplayRequest *record);
/**
 * Provide the workbench context event replay request set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_replay_request_set_source(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *source_id);
/**
 * Provide the workbench context event replay request set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_replay_request_set_subject(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *subject_id);
/**
 * Provide the workbench context event replay request set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_replay_request_set_group(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *group_id);
/**
 * Provide the workbench context event replay request set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_replay_request_set_label(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *label);
/**
 * Provide the workbench context event replay request hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_replay_request_hash(
    const UmiWorkbenchContextEventReplayRequest *record);
/**
 * Provide the workbench context event replay request touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_replay_request_touch(
    UmiWorkbenchContextEventReplayRequest *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
