/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/workspace_binding.h
 *
 * PURPOSE:
 *   Define the reusable workspace selection binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_WORKSPACE_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_WORKSPACE_BINDING_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_selection_workspace_binding_init(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *record_id);
UmiStatus umi_workbench_selection_workspace_binding_validate(
    const UmiWorkbenchSelectionWorkspaceBinding *record);
UmiStatus umi_workbench_selection_workspace_binding_set_source(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *source_id);
UmiStatus umi_workbench_selection_workspace_binding_set_subject(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_workspace_binding_set_secondary(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_workspace_binding_set_group(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *group_id);
UmiStatus umi_workbench_selection_workspace_binding_set_label(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    const char *label);
uint64_t umi_workbench_selection_workspace_binding_hash(
    const UmiWorkbenchSelectionWorkspaceBinding *record);
void umi_workbench_selection_workspace_binding_touch(
    UmiWorkbenchSelectionWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
