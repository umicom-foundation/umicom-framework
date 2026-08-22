/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/duplicate_record.h
 *
 * PURPOSE:
 *   Define the reusable event duplicate record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DUPLICATE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DUPLICATE_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventDuplicateRecord {
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
} UmiWorkbenchContextEventDuplicateRecord;

void umi_workbench_context_event_duplicate_record_init(
    UmiWorkbenchContextEventDuplicateRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_duplicate_record_validate(
    const UmiWorkbenchContextEventDuplicateRecord *record);
UmiStatus umi_workbench_context_event_duplicate_record_set_source(
    UmiWorkbenchContextEventDuplicateRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_duplicate_record_set_subject(
    UmiWorkbenchContextEventDuplicateRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_duplicate_record_set_group(
    UmiWorkbenchContextEventDuplicateRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_duplicate_record_set_label(
    UmiWorkbenchContextEventDuplicateRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_duplicate_record_hash(
    const UmiWorkbenchContextEventDuplicateRecord *record);
void umi_workbench_context_event_duplicate_record_touch(
    UmiWorkbenchContextEventDuplicateRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
