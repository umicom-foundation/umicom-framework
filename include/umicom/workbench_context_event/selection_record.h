/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/selection_record.h
 *
 * PURPOSE:
 *   Define the reusable selection event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SELECTION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SELECTION_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventSelectionRecord {
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
} UmiWorkbenchContextEventSelectionRecord;

void umi_workbench_context_event_selection_record_init(
    UmiWorkbenchContextEventSelectionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_selection_record_validate(
    const UmiWorkbenchContextEventSelectionRecord *record);
UmiStatus umi_workbench_context_event_selection_record_set_source(
    UmiWorkbenchContextEventSelectionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_selection_record_set_subject(
    UmiWorkbenchContextEventSelectionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_selection_record_set_group(
    UmiWorkbenchContextEventSelectionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_selection_record_set_label(
    UmiWorkbenchContextEventSelectionRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_selection_record_hash(
    const UmiWorkbenchContextEventSelectionRecord *record);
void umi_workbench_context_event_selection_record_touch(
    UmiWorkbenchContextEventSelectionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
