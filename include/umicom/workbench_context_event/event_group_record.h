/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/event_group_record.h
 *
 * PURPOSE:
 *   Define the reusable event event group record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_GROUP_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_GROUP_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventEventGroupRecord {
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
} UmiWorkbenchContextEventEventGroupRecord;

void umi_workbench_context_event_event_group_record_init(
    UmiWorkbenchContextEventEventGroupRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_event_group_record_validate(
    const UmiWorkbenchContextEventEventGroupRecord *record);
UmiStatus umi_workbench_context_event_event_group_record_set_source(
    UmiWorkbenchContextEventEventGroupRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_event_group_record_set_subject(
    UmiWorkbenchContextEventEventGroupRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_event_group_record_set_group(
    UmiWorkbenchContextEventEventGroupRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_event_group_record_set_label(
    UmiWorkbenchContextEventEventGroupRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_event_group_record_hash(
    const UmiWorkbenchContextEventEventGroupRecord *record);
void umi_workbench_context_event_event_group_record_touch(
    UmiWorkbenchContextEventEventGroupRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
