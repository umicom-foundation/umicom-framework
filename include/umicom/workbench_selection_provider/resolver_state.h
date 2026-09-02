/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/resolver_state.h
 *
 * PURPOSE:
 *   Define the reusable resolver runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_RESOLVER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_RESOLVER_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider resolver state data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderResolverState {
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
} UmiWorkbenchSelectionProviderResolverState;

/**
 * Initialise workbench selection provider resolver state from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_resolver_state_init(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *record_id);
/**
 * Check that workbench selection provider resolver state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_validate(
    const UmiWorkbenchSelectionProviderResolverState *record);
/**
 * Provide the workbench selection provider resolver state set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_set_provider(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider resolver state set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_set_source(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *source_id);
/**
 * Provide the workbench selection provider resolver state set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_set_subject(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider resolver state set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_set_related(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *related_id);
/**
 * Provide the workbench selection provider resolver state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_set_group(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *group_id);
/**
 * Provide the workbench selection provider resolver state set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_resolver_state_set_description(
    UmiWorkbenchSelectionProviderResolverState *record,
    const char *description);
/**
 * Provide the workbench selection provider resolver state hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_resolver_state_hash(
    const UmiWorkbenchSelectionProviderResolverState *record);
/**
 * Provide the workbench selection provider resolver state touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_resolver_state_touch(
    UmiWorkbenchSelectionProviderResolverState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
