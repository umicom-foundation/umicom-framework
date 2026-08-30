/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/query.h
 *
 * PURPOSE:
 *   Define the reusable event query contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_QUERY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_QUERY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventQuery {
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
} UmiWorkbenchContextEventQuery;

void umi_workbench_context_event_query_init(
    UmiWorkbenchContextEventQuery *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_query_validate(
    const UmiWorkbenchContextEventQuery *record);
UmiStatus umi_workbench_context_event_query_set_source(
    UmiWorkbenchContextEventQuery *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_query_set_subject(
    UmiWorkbenchContextEventQuery *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_query_set_group(
    UmiWorkbenchContextEventQuery *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_query_set_label(
    UmiWorkbenchContextEventQuery *record,
    const char *label);
uint64_t umi_workbench_context_event_query_hash(
    const UmiWorkbenchContextEventQuery *record);
void umi_workbench_context_event_query_touch(
    UmiWorkbenchContextEventQuery *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
