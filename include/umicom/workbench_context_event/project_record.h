/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/project_record.h
 *
 * PURPOSE:
 *   Define the reusable project event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PROJECT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PROJECT_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventProjectRecord {
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
} UmiWorkbenchContextEventProjectRecord;

void umi_workbench_context_event_project_record_init(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_project_record_validate(
    const UmiWorkbenchContextEventProjectRecord *record);
UmiStatus umi_workbench_context_event_project_record_set_source(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_project_record_set_subject(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_project_record_set_group(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_project_record_set_label(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_project_record_hash(
    const UmiWorkbenchContextEventProjectRecord *record);
void umi_workbench_context_event_project_record_touch(
    UmiWorkbenchContextEventProjectRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
