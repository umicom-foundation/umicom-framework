/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/trace.h
 *
 * PURPOSE:
 *   Define the reusable event trace contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_TRACE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_TRACE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventTrace {
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
} UmiWorkbenchContextEventTrace;

void umi_workbench_context_event_trace_init(
    UmiWorkbenchContextEventTrace *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_trace_validate(
    const UmiWorkbenchContextEventTrace *record);
UmiStatus umi_workbench_context_event_trace_set_source(
    UmiWorkbenchContextEventTrace *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_trace_set_subject(
    UmiWorkbenchContextEventTrace *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_trace_set_group(
    UmiWorkbenchContextEventTrace *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_trace_set_label(
    UmiWorkbenchContextEventTrace *record,
    const char *label);
uint64_t umi_workbench_context_event_trace_hash(
    const UmiWorkbenchContextEventTrace *record);
void umi_workbench_context_event_trace_touch(
    UmiWorkbenchContextEventTrace *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
