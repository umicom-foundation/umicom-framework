/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/panel_binding.h
 *
 * PURPOSE:
 *   Define the reusable provider-to-panel binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PANEL_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PANEL_BINDING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider panel binding data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderPanelBinding {
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
} UmiWorkbenchSelectionProviderPanelBinding;

/**
 * Initialise workbench selection provider panel binding from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_panel_binding_init(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *record_id);
/**
 * Check that workbench selection provider panel binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_validate(
    const UmiWorkbenchSelectionProviderPanelBinding *record);
/**
 * Provide the workbench selection provider panel binding set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_set_provider(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider panel binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_set_source(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection provider panel binding set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_set_subject(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider panel binding set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_set_related(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *related_id);
/**
 * Provide the workbench selection provider panel binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_set_group(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection provider panel binding set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_panel_binding_set_description(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    const char *description);
/**
 * Provide the workbench selection provider panel binding hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_panel_binding_hash(
    const UmiWorkbenchSelectionProviderPanelBinding *record);
/**
 * Provide the workbench selection provider panel binding touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_panel_binding_touch(
    UmiWorkbenchSelectionProviderPanelBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
