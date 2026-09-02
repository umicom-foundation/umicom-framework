/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_binding.h
 *
 * PURPOSE:
 *   Define the reusable provider-to-source binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_BINDING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider source binding data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSourceBinding {
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
} UmiWorkbenchSelectionProviderSourceBinding;

/**
 * Initialise workbench selection provider source binding from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_source_binding_init(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *record_id);
/**
 * Check that workbench selection provider source binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_source_binding_validate(
    const UmiWorkbenchSelectionProviderSourceBinding *record);
/**
 * Provide the workbench selection provider source binding set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_binding_set_provider(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider source binding set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_binding_set_source(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection provider source binding set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_binding_set_subject(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider source binding set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_binding_set_related(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *related_id);
/**
 * Provide the workbench selection provider source binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_binding_set_group(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection provider source binding set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_binding_set_description(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    const char *description);
/**
 * Provide the workbench selection provider source binding hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_source_binding_hash(
    const UmiWorkbenchSelectionProviderSourceBinding *record);
/**
 * Provide the workbench selection provider source binding touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_source_binding_touch(
    UmiWorkbenchSelectionProviderSourceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
