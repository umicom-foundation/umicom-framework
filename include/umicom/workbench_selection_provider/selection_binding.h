/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/selection_binding.h
 *
 * PURPOSE:
 *   Define the reusable provider-to-selection-kind binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_BINDING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSelectionBinding {
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
} UmiWorkbenchSelectionProviderSelectionBinding;

void umi_workbench_selection_provider_selection_binding_init(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_selection_binding_validate(
    const UmiWorkbenchSelectionProviderSelectionBinding *record);
UmiStatus umi_workbench_selection_provider_selection_binding_set_provider(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_selection_binding_set_source(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_selection_binding_set_subject(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_selection_binding_set_related(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_selection_binding_set_group(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_selection_binding_set_description(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    const char *description);
uint64_t umi_workbench_selection_provider_selection_binding_hash(
    const UmiWorkbenchSelectionProviderSelectionBinding *record);
void umi_workbench_selection_provider_selection_binding_touch(
    UmiWorkbenchSelectionProviderSelectionBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
