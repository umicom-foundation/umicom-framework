/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/signal_binding.h
 *
 * PURPOSE:
 *   Define the reusable native signal binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SIGNAL_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SIGNAL_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source signal binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceSignalBinding {
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
} UmiWorkbenchContextSourceSignalBinding;

/**
 * Initialise workbench context source signal binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_signal_binding_init(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *record_id);
/**
 * Check that workbench context source signal binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_signal_binding_validate(
    const UmiWorkbenchContextSourceSignalBinding *record);
/**
 * Provide the workbench context source signal binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_signal_binding_set_source(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *source_id);
/**
 * Provide the workbench context source signal binding set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_signal_binding_set_panel(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *panel_id);
/**
 * Provide the workbench context source signal binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_signal_binding_set_subject(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context source signal binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_signal_binding_set_group(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *group_id);
/**
 * Provide the workbench context source signal binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_signal_binding_set_label(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *label);
/**
 * Provide the workbench context source signal binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_signal_binding_hash(
    const UmiWorkbenchContextSourceSignalBinding *record);
/**
 * Provide the workbench context source signal binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_signal_binding_touch(
    UmiWorkbenchContextSourceSignalBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
