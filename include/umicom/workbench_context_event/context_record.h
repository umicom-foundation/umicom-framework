/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/context_record.h
 *
 * PURPOSE:
 *   Define the reusable context publication record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_CONTEXT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_CONTEXT_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventContextRecord {
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
} UmiWorkbenchContextEventContextRecord;

void umi_workbench_context_event_context_record_init(
    UmiWorkbenchContextEventContextRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_context_record_validate(
    const UmiWorkbenchContextEventContextRecord *record);
UmiStatus umi_workbench_context_event_context_record_set_source(
    UmiWorkbenchContextEventContextRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_context_record_set_subject(
    UmiWorkbenchContextEventContextRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_context_record_set_group(
    UmiWorkbenchContextEventContextRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_context_record_set_label(
    UmiWorkbenchContextEventContextRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_context_record_hash(
    const UmiWorkbenchContextEventContextRecord *record);
void umi_workbench_context_event_context_record_touch(
    UmiWorkbenchContextEventContextRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
