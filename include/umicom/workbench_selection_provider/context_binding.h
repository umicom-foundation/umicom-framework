/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/context_binding.h
 *
 * PURPOSE:
 *   Define the reusable provider-to-context-kind binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CONTEXT_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CONTEXT_BINDING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider context binding data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderContextBinding {
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
} UmiWorkbenchSelectionProviderContextBinding;

/**
 * Initialise workbench selection provider context binding from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_context_binding_init(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *record_id);
/**
 * Check that workbench selection provider context binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_context_binding_validate(
    const UmiWorkbenchSelectionProviderContextBinding *record);
/**
 * Provide the workbench selection provider context binding set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_context_binding_set_provider(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider context binding set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_context_binding_set_source(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection provider context binding set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_context_binding_set_subject(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider context binding set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_context_binding_set_related(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *related_id);
/**
 * Provide the workbench selection provider context binding set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_context_binding_set_group(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection provider context binding set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_context_binding_set_description(
    UmiWorkbenchSelectionProviderContextBinding *record,
    const char *description);
/**
 * Provide the workbench selection provider context binding hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_context_binding_hash(
    const UmiWorkbenchSelectionProviderContextBinding *record);
/**
 * Provide the workbench selection provider context binding touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_context_binding_touch(
    UmiWorkbenchSelectionProviderContextBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
