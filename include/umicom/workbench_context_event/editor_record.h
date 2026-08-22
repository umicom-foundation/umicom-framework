/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/editor_record.h
 *
 * PURPOSE:
 *   Define the reusable editor event record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_EDITOR_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_EDITOR_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventEditorRecord {
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
} UmiWorkbenchContextEventEditorRecord;

void umi_workbench_context_event_editor_record_init(
    UmiWorkbenchContextEventEditorRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_editor_record_validate(
    const UmiWorkbenchContextEventEditorRecord *record);
UmiStatus umi_workbench_context_event_editor_record_set_source(
    UmiWorkbenchContextEventEditorRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_editor_record_set_subject(
    UmiWorkbenchContextEventEditorRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_editor_record_set_group(
    UmiWorkbenchContextEventEditorRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_editor_record_set_label(
    UmiWorkbenchContextEventEditorRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_editor_record_hash(
    const UmiWorkbenchContextEventEditorRecord *record);
void umi_workbench_context_event_editor_record_touch(
    UmiWorkbenchContextEventEditorRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
