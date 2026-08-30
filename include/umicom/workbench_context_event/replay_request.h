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

void umi_workbench_context_event_replay_request_init(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_replay_request_validate(
    const UmiWorkbenchContextEventReplayRequest *record);
UmiStatus umi_workbench_context_event_replay_request_set_source(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_replay_request_set_subject(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_replay_request_set_group(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_replay_request_set_label(
    UmiWorkbenchContextEventReplayRequest *record,
    const char *label);
uint64_t umi_workbench_context_event_replay_request_hash(
    const UmiWorkbenchContextEventReplayRequest *record);
void umi_workbench_context_event_replay_request_touch(
    UmiWorkbenchContextEventReplayRequest *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
