/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/problem_mapping.h
 *
 * PURPOSE:
 *   Define the reusable problem model-to-selection mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderProblemMapping {
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
} UmiWorkbenchSelectionProviderProblemMapping;

void umi_workbench_selection_provider_problem_mapping_init(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_problem_mapping_validate(
    const UmiWorkbenchSelectionProviderProblemMapping *record);
UmiStatus umi_workbench_selection_provider_problem_mapping_set_provider(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_problem_mapping_set_source(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_problem_mapping_set_subject(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_problem_mapping_set_related(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_problem_mapping_set_group(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_problem_mapping_set_description(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    const char *description);
uint64_t umi_workbench_selection_provider_problem_mapping_hash(
    const UmiWorkbenchSelectionProviderProblemMapping *record);
void umi_workbench_selection_provider_problem_mapping_touch(
    UmiWorkbenchSelectionProviderProblemMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
