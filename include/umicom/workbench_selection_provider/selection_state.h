/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/selection_state.h
 *
 * PURPOSE:
 *   Define the reusable provider selection runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSelectionState {
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
} UmiWorkbenchSelectionProviderSelectionState;

void umi_workbench_selection_provider_selection_state_init(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_selection_state_validate(
    const UmiWorkbenchSelectionProviderSelectionState *record);
UmiStatus umi_workbench_selection_provider_selection_state_set_provider(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_selection_state_set_source(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_selection_state_set_subject(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_selection_state_set_related(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_selection_state_set_group(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_selection_state_set_description(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *description);
uint64_t umi_workbench_selection_provider_selection_state_hash(
    const UmiWorkbenchSelectionProviderSelectionState *record);
void umi_workbench_selection_provider_selection_state_touch(
    UmiWorkbenchSelectionProviderSelectionState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
