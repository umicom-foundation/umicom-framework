/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_commit_mapping.h
 *
 * PURPOSE:
 *   Define the reusable source-control commit mapping contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_COMMIT_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_COMMIT_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSourceCommitMapping {
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
} UmiWorkbenchSelectionProviderSourceCommitMapping;

void umi_workbench_selection_provider_source_commit_mapping_init(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_validate(
    const UmiWorkbenchSelectionProviderSourceCommitMapping *record);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_provider(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_source(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_subject(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_related(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_group(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_description(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *description);
uint64_t umi_workbench_selection_provider_source_commit_mapping_hash(
    const UmiWorkbenchSelectionProviderSourceCommitMapping *record);
void umi_workbench_selection_provider_source_commit_mapping_touch(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
