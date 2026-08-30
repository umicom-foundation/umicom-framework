/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/provider_state.h
 *
 * PURPOSE:
 *   Define the reusable provider runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROVIDER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROVIDER_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderProviderState {
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
} UmiWorkbenchSelectionProviderProviderState;

void umi_workbench_selection_provider_provider_state_init(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_provider_state_validate(
    const UmiWorkbenchSelectionProviderProviderState *record);
UmiStatus umi_workbench_selection_provider_provider_state_set_provider(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_provider_state_set_source(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_provider_state_set_subject(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_provider_state_set_related(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_provider_state_set_group(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_provider_state_set_description(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *description);
uint64_t umi_workbench_selection_provider_provider_state_hash(
    const UmiWorkbenchSelectionProviderProviderState *record);
void umi_workbench_selection_provider_provider_state_touch(
    UmiWorkbenchSelectionProviderProviderState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
