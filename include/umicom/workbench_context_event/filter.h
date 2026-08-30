/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/filter.h
 *
 * PURPOSE:
 *   Define the reusable event filter contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_FILTER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_FILTER_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventFilter {
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
} UmiWorkbenchContextEventFilter;

void umi_workbench_context_event_filter_init(
    UmiWorkbenchContextEventFilter *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_filter_validate(
    const UmiWorkbenchContextEventFilter *record);
UmiStatus umi_workbench_context_event_filter_set_source(
    UmiWorkbenchContextEventFilter *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_filter_set_subject(
    UmiWorkbenchContextEventFilter *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_filter_set_group(
    UmiWorkbenchContextEventFilter *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_filter_set_label(
    UmiWorkbenchContextEventFilter *record,
    const char *label);
uint64_t umi_workbench_context_event_filter_hash(
    const UmiWorkbenchContextEventFilter *record);
void umi_workbench_context_event_filter_touch(
    UmiWorkbenchContextEventFilter *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
