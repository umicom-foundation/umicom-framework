/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/workspace_binding.h
 *
 * PURPOSE:
 *   Define the reusable workspace selection binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_WORKSPACE_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_WORKSPACE_BINDING_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection workspace binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionWorkspaceBinding {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionWorkspaceBinding;

/**
 * Initialise workbench selection workspace binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_workspace_binding_init(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *record_id);
/**
 * Check that workbench selection workspace binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_workspace_binding_validate(
    const UmiWorkbenchSelectionWorkspaceBinding *record);
/**
 * Provide the workbench selection workspace binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_workspace_binding_set_source(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection workspace binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_workspace_binding_set_subject(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection workspace binding set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_workspace_binding_set_secondary(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *secondary_id);
/**
 * Provide the workbench selection workspace binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_workspace_binding_set_group(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection workspace binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_workspace_binding_set_label(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *label);
/**
 * Provide the workbench selection workspace binding hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_workspace_binding_hash(
    const UmiWorkbenchSelectionWorkspaceBinding *record);
/**
 * Provide the workbench selection workspace binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_workspace_binding_touch(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
