/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/field_requirement.h
 *
 * PURPOSE:
 *   Define the reusable structured field requirement contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_REQUIREMENT_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_REQUIREMENT_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderFieldRequirement {
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
} UmiWorkbenchSelectionProviderFieldRequirement;

void umi_workbench_selection_provider_field_requirement_init(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_field_requirement_validate(
    const UmiWorkbenchSelectionProviderFieldRequirement *record);
UmiStatus umi_workbench_selection_provider_field_requirement_set_provider(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_field_requirement_set_source(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_field_requirement_set_subject(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_field_requirement_set_related(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_field_requirement_set_group(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_field_requirement_set_description(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *description);
uint64_t umi_workbench_selection_provider_field_requirement_hash(
    const UmiWorkbenchSelectionProviderFieldRequirement *record);
void umi_workbench_selection_provider_field_requirement_touch(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
