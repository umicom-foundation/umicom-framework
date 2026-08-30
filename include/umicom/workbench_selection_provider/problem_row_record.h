/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/problem_row_record.h
 *
 * PURPOSE:
 *   Define the reusable structured problem row record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_ROW_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_ROW_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderProblemRowRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderProblemRowRecord;

void umi_workbench_selection_provider_problem_row_record_init(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_problem_row_record_validate(
    const UmiWorkbenchSelectionProviderProblemRowRecord *record);
UmiStatus umi_workbench_selection_provider_problem_row_record_set_provider(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_problem_row_record_set_source(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_problem_row_record_set_subject(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_problem_row_record_set_related(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_problem_row_record_set_group(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_problem_row_record_set_description(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_problem_row_record_hash(
    const UmiWorkbenchSelectionProviderProblemRowRecord *record);
void umi_workbench_selection_provider_problem_row_record_touch(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
