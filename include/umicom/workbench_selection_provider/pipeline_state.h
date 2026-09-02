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

/**
 * Represent the workbench selection provider pipeline state data shared with callers of
 * this public contract.
 */
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

/**
 * Initialise workbench selection provider pipeline state from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_pipeline_state_init(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *record_id);
/**
 * Check that workbench selection provider pipeline state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_validate(
    const UmiWorkbenchSelectionProviderPipelineState *record);
/**
 * Provide the workbench selection provider pipeline state set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_set_provider(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider pipeline state set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_set_source(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *source_id);
/**
 * Provide the workbench selection provider pipeline state set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_set_subject(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider pipeline state set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_set_related(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *related_id);
/**
 * Provide the workbench selection provider pipeline state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_set_group(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *group_id);
/**
 * Provide the workbench selection provider pipeline state set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_pipeline_state_set_description(
    UmiWorkbenchSelectionProviderPipelineState *record,
    const char *description);
/**
 * Provide the workbench selection provider pipeline state hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_pipeline_state_hash(
    const UmiWorkbenchSelectionProviderPipelineState *record);
/**
 * Provide the workbench selection provider pipeline state touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_pipeline_state_touch(
    UmiWorkbenchSelectionProviderPipelineState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
