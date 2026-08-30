/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/counter.h
 *
 * PURPOSE:
 *   Define the reusable event counter contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COUNTER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COUNTER_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventCounter {
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
} UmiWorkbenchContextEventCounter;

void umi_workbench_context_event_counter_init(
    UmiWorkbenchContextEventCounter *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_counter_validate(
    const UmiWorkbenchContextEventCounter *record);
UmiStatus umi_workbench_context_event_counter_set_source(
    UmiWorkbenchContextEventCounter *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_counter_set_subject(
    UmiWorkbenchContextEventCounter *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_counter_set_group(
    UmiWorkbenchContextEventCounter *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_counter_set_label(
    UmiWorkbenchContextEventCounter *record,
    const char *label);
uint64_t umi_workbench_context_event_counter_hash(
    const UmiWorkbenchContextEventCounter *record);
void umi_workbench_context_event_counter_touch(
    UmiWorkbenchContextEventCounter *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
