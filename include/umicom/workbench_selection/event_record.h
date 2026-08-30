/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/event_record.h
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

#ifndef UMICOM_WORKBENCH_SELECTION_EVENT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_EVENT_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionEventRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionEventRecord;

void umi_workbench_selection_event_record_init(
    UmiWorkbenchSelectionEventRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_event_record_validate(
    const UmiWorkbenchSelectionEventRecord *record);
UmiStatus umi_workbench_selection_event_record_set_source(
    UmiWorkbenchSelectionEventRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_event_record_set_subject(
    UmiWorkbenchSelectionEventRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_event_record_set_related(
    UmiWorkbenchSelectionEventRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_event_record_set_group(
    UmiWorkbenchSelectionEventRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_event_record_set_description(
    UmiWorkbenchSelectionEventRecord *record,
    const char *description);
uint64_t umi_workbench_selection_event_record_hash(
    const UmiWorkbenchSelectionEventRecord *record);
void umi_workbench_selection_event_record_touch(
    UmiWorkbenchSelectionEventRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
