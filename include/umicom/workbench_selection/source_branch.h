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

/**
 * Represent the workbench selection source branch data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench selection source branch from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_source_branch_init(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *record_id);
/**
 * Check that workbench selection source branch satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_source_branch_validate(
    const UmiWorkbenchSelectionSourceBranch *record);
/**
 * Provide the workbench selection source branch set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_branch_set_source(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *source_id);
/**
 * Provide the workbench selection source branch set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_branch_set_subject(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *subject_id);
/**
 * Provide the workbench selection source branch set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_source_branch_set_secondary(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *secondary_id);
/**
 * Provide the workbench selection source branch set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_branch_set_group(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *group_id);
/**
 * Provide the workbench selection source branch set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_branch_set_label(
    UmiWorkbenchSelectionSourceBranch *record,
    const char *label);
/**
 * Provide the workbench selection source branch hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_source_branch_hash(
    const UmiWorkbenchSelectionSourceBranch *record);
/**
 * Provide the workbench selection source branch touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_source_branch_touch(
    UmiWorkbenchSelectionSourceBranch *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
