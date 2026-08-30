/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/source_branch.h
 *
 * PURPOSE:
 *   Define the reusable structured source-control branch contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SOURCE_BRANCH_H
#define UMICOM_WORKBENCH_SELECTION_SOURCE_BRANCH_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSourceBranch {
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
} UmiWorkbenchSelectionSourceBranch;

void umi_workbench_selection_source_branch_init(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *record_id);
UmiStatus umi_workbench_selection_source_branch_validate(
    const UmiWorkbenchSelectionSourceBranch *record);
UmiStatus umi_workbench_selection_source_branch_set_source(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *source_id);
UmiStatus umi_workbench_selection_source_branch_set_subject(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_source_branch_set_secondary(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_source_branch_set_group(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *group_id);
UmiStatus umi_workbench_selection_source_branch_set_label(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *label);
uint64_t umi_workbench_selection_source_branch_hash(
    const UmiWorkbenchSelectionSourceBranch *record);
void umi_workbench_selection_source_branch_touch(
    UmiWorkbenchSelectionSourceBranch *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
