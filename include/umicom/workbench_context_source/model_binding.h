/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/model_binding.h
 *
 * PURPOSE:
 *   Define the reusable toolkit-neutral model binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_MODEL_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_MODEL_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source model binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceModelBinding {
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
} UmiWorkbenchContextSourceModelBinding;

/**
 * Initialise workbench context source model binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_model_binding_init(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *record_id);
/**
 * Check that workbench context source model binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_model_binding_validate(
    const UmiWorkbenchContextSourceModelBinding *record);
/**
 * Provide the workbench context source model binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_model_binding_set_source(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *source_id);
/**
 * Provide the workbench context source model binding set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_model_binding_set_panel(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *panel_id);
/**
 * Provide the workbench context source model binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_model_binding_set_subject(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context source model binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_model_binding_set_group(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *group_id);
/**
 * Provide the workbench context source model binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_model_binding_set_label(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *label);
/**
 * Provide the workbench context source model binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_model_binding_hash(
    const UmiWorkbenchContextSourceModelBinding *record);
/**
 * Provide the workbench context source model binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_model_binding_touch(
    UmiWorkbenchContextSourceModelBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
