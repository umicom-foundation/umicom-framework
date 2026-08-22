/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/refresh_record.h
 *
 * PURPOSE:
 *   Define the reusable selection refresh record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_REFRESH_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_REFRESH_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionRefreshRecord {
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
} UmiWorkbenchSelectionRefreshRecord;

void umi_workbench_selection_refresh_record_init(
    UmiWorkbenchSelectionRefreshRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_refresh_record_validate(
    const UmiWorkbenchSelectionRefreshRecord *record);
UmiStatus umi_workbench_selection_refresh_record_set_source(
    UmiWorkbenchSelectionRefreshRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_refresh_record_set_subject(
    UmiWorkbenchSelectionRefreshRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_refresh_record_set_related(
    UmiWorkbenchSelectionRefreshRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_refresh_record_set_group(
    UmiWorkbenchSelectionRefreshRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_refresh_record_set_description(
    UmiWorkbenchSelectionRefreshRecord *record,
    const char *description);
uint64_t umi_workbench_selection_refresh_record_hash(
    const UmiWorkbenchSelectionRefreshRecord *record);
void umi_workbench_selection_refresh_record_touch(
    UmiWorkbenchSelectionRefreshRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
