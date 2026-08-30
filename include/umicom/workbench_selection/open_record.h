/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/open_record.h
 *
 * PURPOSE:
 *   Define the reusable selection open record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_OPEN_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_OPEN_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionOpenRecord {
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
} UmiWorkbenchSelectionOpenRecord;

void umi_workbench_selection_open_record_init(
    UmiWorkbenchSelectionOpenRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_open_record_validate(
    const UmiWorkbenchSelectionOpenRecord *record);
UmiStatus umi_workbench_selection_open_record_set_source(
    UmiWorkbenchSelectionOpenRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_open_record_set_subject(
    UmiWorkbenchSelectionOpenRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_open_record_set_related(
    UmiWorkbenchSelectionOpenRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_open_record_set_group(
    UmiWorkbenchSelectionOpenRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_open_record_set_description(
    UmiWorkbenchSelectionOpenRecord *record,
    const char *description);
uint64_t umi_workbench_selection_open_record_hash(
    const UmiWorkbenchSelectionOpenRecord *record);
void umi_workbench_selection_open_record_touch(
    UmiWorkbenchSelectionOpenRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
