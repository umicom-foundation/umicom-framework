/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/group_binding.h
 *
 * PURPOSE:
 *   Define the reusable provider-to-context-group binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_GROUP_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_GROUP_BINDING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider group binding data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderGroupBinding {
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
} UmiWorkbenchSelectionProviderGroupBinding;

/**
 * Initialise workbench selection provider group binding from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_group_binding_init(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *record_id);
/**
 * Check that workbench selection provider group binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_group_binding_validate(
    const UmiWorkbenchSelectionProviderGroupBinding *record);
/**
 * Provide the workbench selection provider group binding set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_group_binding_set_provider(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider group binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_group_binding_set_source(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection provider group binding set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_group_binding_set_subject(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider group binding set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_group_binding_set_related(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *related_id);
/**
 * Provide the workbench selection provider group binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_group_binding_set_group(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection provider group binding set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_group_binding_set_description(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    const char *description);
/**
 * Provide the workbench selection provider group binding hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_group_binding_hash(
    const UmiWorkbenchSelectionProviderGroupBinding *record);
/**
 * Provide the workbench selection provider group binding touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_group_binding_touch(
    UmiWorkbenchSelectionProviderGroupBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
