/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/merge.h
 *
 * PURPOSE:
 *   Perform deterministic three-way layout merges and expose conflicts instead of silently discarding user or application changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_MERGE_H
#define UMICOM_WORKBENCH_LAYOUT_MERGE_H

#include "umicom/workbench_layout/diff.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench layout merge resolution values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutMergeResolution {
    UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED = 0,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_BASE = 1,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_LOCAL = 2,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_REMOTE = 3,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM = 4
} UmiWorkbenchLayoutMergeResolution;

/**
 * Represent the workbench layout merge conflict data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutMergeConflict {
    uint32_t structure_size;
    char conflict_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutConflictKind kind;
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char field[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char base_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    char local_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    char remote_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    UmiWorkbenchLayoutMergeResolution resolution;
    char custom_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
} UmiWorkbenchLayoutMergeConflict;

/**
 * Represent the workbench layout merge plan data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutMergePlan {
    uint32_t structure_size;
    UmiWorkbenchLayoutMergeConflict
        conflicts[UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS];
    size_t conflict_count;
    size_t automatically_merged_count;
    bool complete;
} UmiWorkbenchLayoutMergePlan;

/**
 * Represent the workbench layout merge options data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutMergeOptions {
    uint32_t structure_size;
    bool prefer_local_for_metadata;
    bool preserve_remote_additions;
    bool preserve_local_additions;
    bool reject_unresolved_conflicts;
} UmiWorkbenchLayoutMergeOptions;

/**
 * Provide the workbench layout merge options default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutMergeOptions
umi_workbench_layout_merge_options_default(void);

/**
 * Initialise workbench layout merge plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_merge_plan_init(
    UmiWorkbenchLayoutMergePlan *plan);

/**
 * Provide the workbench layout merge three way operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_merge_three_way(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergeOptions *options,
    UmiWorkbenchLayoutMergePlan *out_plan,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout merge resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_merge_resolve(
    UmiWorkbenchLayoutMergePlan *plan,
    const char *conflict_id,
    UmiWorkbenchLayoutMergeResolution resolution,
    const char *custom_value);

/**
 * Provide the workbench layout merge apply resolutions operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_merge_apply_resolutions(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Find workbench layout merge conflict while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutMergeConflict *
umi_workbench_layout_merge_conflict_at(
    const UmiWorkbenchLayoutMergePlan *plan,
    size_t index);

/**
 * Return the number of records represented by workbench layout merge unresolved without
 * changing their state.
 */
size_t umi_workbench_layout_merge_unresolved_count(
    const UmiWorkbenchLayoutMergePlan *plan);

#ifdef __cplusplus
}
#endif

#endif
