/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/pipeline_state.h
 *
 * PURPOSE:
 *   Define the reusable provider pipeline runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PIPELINE_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PIPELINE_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderPipelineState {
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
} UmiWorkbenchSelectionProviderPipelineState;

void umi_workbench_selection_provider_pipeline_state_init(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_pipeline_state_validate(
    const UmiWorkbenchSelectionProviderPipelineState *record);
UmiStatus umi_workbench_selection_provider_pipeline_state_set_provider(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_pipeline_state_set_source(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_pipeline_state_set_subject(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_pipeline_state_set_related(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_pipeline_state_set_group(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_pipeline_state_set_description(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *description);
uint64_t umi_workbench_selection_provider_pipeline_state_hash(
    const UmiWorkbenchSelectionProviderPipelineState *record);
void umi_workbench_selection_provider_pipeline_state_touch(
    UmiWorkbenchSelectionProviderPipelineState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
