/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/field_mapping.h
 *
 * PURPOSE:
 *   Define the reusable structured field mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderFieldMapping {
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
} UmiWorkbenchSelectionProviderFieldMapping;

void umi_workbench_selection_provider_field_mapping_init(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_field_mapping_validate(
    const UmiWorkbenchSelectionProviderFieldMapping *record);
UmiStatus umi_workbench_selection_provider_field_mapping_set_provider(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_field_mapping_set_source(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_field_mapping_set_subject(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_field_mapping_set_related(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_field_mapping_set_group(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_field_mapping_set_description(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    const char *description);
uint64_t umi_workbench_selection_provider_field_mapping_hash(
    const UmiWorkbenchSelectionProviderFieldMapping *record);
void umi_workbench_selection_provider_field_mapping_touch(
    UmiWorkbenchSelectionProviderFieldMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
