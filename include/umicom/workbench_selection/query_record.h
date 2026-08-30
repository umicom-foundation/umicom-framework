/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/query_record.h
 *
 * PURPOSE:
 *   Define the reusable selection query record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_QUERY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_QUERY_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionQueryRecord {
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
} UmiWorkbenchSelectionQueryRecord;

void umi_workbench_selection_query_record_init(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_query_record_validate(
    const UmiWorkbenchSelectionQueryRecord *record);
UmiStatus umi_workbench_selection_query_record_set_source(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_query_record_set_subject(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_query_record_set_related(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_query_record_set_group(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_query_record_set_description(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *description);
uint64_t umi_workbench_selection_query_record_hash(
    const UmiWorkbenchSelectionQueryRecord *record);
void umi_workbench_selection_query_record_touch(
    UmiWorkbenchSelectionQueryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
