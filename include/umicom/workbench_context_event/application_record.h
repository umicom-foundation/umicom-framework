/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/application_record.h
 *
 * PURPOSE:
 *   Define the reusable application event record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_APPLICATION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_APPLICATION_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventApplicationRecord {
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
} UmiWorkbenchContextEventApplicationRecord;

void umi_workbench_context_event_application_record_init(
    UmiWorkbenchContextEventApplicationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_application_record_validate(
    const UmiWorkbenchContextEventApplicationRecord *record);
UmiStatus umi_workbench_context_event_application_record_set_source(
    UmiWorkbenchContextEventApplicationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_application_record_set_subject(
    UmiWorkbenchContextEventApplicationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_application_record_set_group(
    UmiWorkbenchContextEventApplicationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_application_record_set_label(
    UmiWorkbenchContextEventApplicationRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_application_record_hash(
    const UmiWorkbenchContextEventApplicationRecord *record);
void umi_workbench_context_event_application_record_touch(
    UmiWorkbenchContextEventApplicationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
