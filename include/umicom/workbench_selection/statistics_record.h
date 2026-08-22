/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/statistics_record.h
 *
 * PURPOSE:
 *   Define the reusable selection statistics record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_STATISTICS_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_STATISTICS_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionStatisticsRecord {
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
} UmiWorkbenchSelectionStatisticsRecord;

void umi_workbench_selection_statistics_record_init(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_statistics_record_validate(
    const UmiWorkbenchSelectionStatisticsRecord *record);
UmiStatus umi_workbench_selection_statistics_record_set_source(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_statistics_record_set_subject(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_statistics_record_set_related(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_statistics_record_set_group(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_statistics_record_set_description(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *description);
uint64_t umi_workbench_selection_statistics_record_hash(
    const UmiWorkbenchSelectionStatisticsRecord *record);
void umi_workbench_selection_statistics_record_touch(
    UmiWorkbenchSelectionStatisticsRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
