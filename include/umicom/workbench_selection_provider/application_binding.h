/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/application_binding.h
 *
 * PURPOSE:
 *   Define the reusable provider-to-application binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_APPLICATION_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_APPLICATION_BINDING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider application binding data shared with callers
 * of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderApplicationBinding {
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
} UmiWorkbenchSelectionProviderApplicationBinding;

/**
 * Initialise workbench selection provider application binding from caller-provided values
 * so later operations receive a known state.
 */
void umi_workbench_selection_provider_application_binding_init(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *record_id);
/**
 * Check that workbench selection provider application binding satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_application_binding_validate(
    const UmiWorkbenchSelectionProviderApplicationBinding *record);
/**
 * Provide the workbench selection provider application binding set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_application_binding_set_provider(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider application binding set source operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_application_binding_set_source(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection provider application binding set subject operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_application_binding_set_subject(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider application binding set related operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_application_binding_set_related(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *related_id);
/**
 * Provide the workbench selection provider application binding set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_application_binding_set_group(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection provider application binding set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_application_binding_set_description(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    const char *description);
/**
 * Provide the workbench selection provider application binding hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_application_binding_hash(
    const UmiWorkbenchSelectionProviderApplicationBinding *record);
/**
 * Provide the workbench selection provider application binding touch operation used by
 * this module and its client applications.
 */
void umi_workbench_selection_provider_application_binding_touch(
    UmiWorkbenchSelectionProviderApplicationBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
