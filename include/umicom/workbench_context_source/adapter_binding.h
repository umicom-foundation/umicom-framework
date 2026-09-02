/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/adapter_binding.h
 *
 * PURPOSE:
 *   Define the reusable frontend adapter binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_ADAPTER_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_ADAPTER_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source adapter binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceAdapterBinding {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceAdapterBinding;

/**
 * Initialise workbench context source adapter binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_adapter_binding_init(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *record_id);
/**
 * Check that workbench context source adapter binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_adapter_binding_validate(
    const UmiWorkbenchContextSourceAdapterBinding *record);
/**
 * Provide the workbench context source adapter binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_adapter_binding_set_source(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *source_id);
/**
 * Provide the workbench context source adapter binding set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_adapter_binding_set_panel(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *panel_id);
/**
 * Provide the workbench context source adapter binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_adapter_binding_set_subject(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context source adapter binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_adapter_binding_set_group(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *group_id);
/**
 * Provide the workbench context source adapter binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_adapter_binding_set_label(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *label);
/**
 * Provide the workbench context source adapter binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_adapter_binding_hash(
    const UmiWorkbenchContextSourceAdapterBinding *record);
/**
 * Provide the workbench context source adapter binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_adapter_binding_touch(
    UmiWorkbenchContextSourceAdapterBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
