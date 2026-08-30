/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/project_mapping.h
 *
 * PURPOSE:
 *   Define the reusable project model-to-selection mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECT_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECT_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderProjectMapping {
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
} UmiWorkbenchSelectionProviderProjectMapping;

void umi_workbench_selection_provider_project_mapping_init(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_project_mapping_validate(
    const UmiWorkbenchSelectionProviderProjectMapping *record);
UmiStatus umi_workbench_selection_provider_project_mapping_set_provider(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_project_mapping_set_source(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_project_mapping_set_subject(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_project_mapping_set_related(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_project_mapping_set_group(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_project_mapping_set_description(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    const char *description);
uint64_t umi_workbench_selection_provider_project_mapping_hash(
    const UmiWorkbenchSelectionProviderProjectMapping *record);
void umi_workbench_selection_provider_project_mapping_touch(
    UmiWorkbenchSelectionProviderProjectMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
