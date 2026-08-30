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

typedef enum UmiWorkbenchLayoutMergeResolution {
    UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED = 0,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_BASE = 1,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_LOCAL = 2,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_REMOTE = 3,
    UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM = 4
} UmiWorkbenchLayoutMergeResolution;

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

typedef struct UmiWorkbenchLayoutMergePlan {
    uint32_t structure_size;
    UmiWorkbenchLayoutMergeConflict
        conflicts[UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS];
    size_t conflict_count;
    size_t automatically_merged_count;
    bool complete;
} UmiWorkbenchLayoutMergePlan;

typedef struct UmiWorkbenchLayoutMergeOptions {
    uint32_t structure_size;
    bool prefer_local_for_metadata;
    bool preserve_remote_additions;
    bool preserve_local_additions;
    bool reject_unresolved_conflicts;
} UmiWorkbenchLayoutMergeOptions;

UmiWorkbenchLayoutMergeOptions
umi_workbench_layout_merge_options_default(void);

void umi_workbench_layout_merge_plan_init(
    UmiWorkbenchLayoutMergePlan *plan);

UmiStatus umi_workbench_layout_merge_three_way(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergeOptions *options,
    UmiWorkbenchLayoutMergePlan *out_plan,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_merge_resolve(
    UmiWorkbenchLayoutMergePlan *plan,
    const char *conflict_id,
    UmiWorkbenchLayoutMergeResolution resolution,
    const char *custom_value);

UmiStatus umi_workbench_layout_merge_apply_resolutions(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutDocument *out_document);

const UmiWorkbenchLayoutMergeConflict *
umi_workbench_layout_merge_conflict_at(
    const UmiWorkbenchLayoutMergePlan *plan,
    size_t index);

size_t umi_workbench_layout_merge_unresolved_count(
    const UmiWorkbenchLayoutMergePlan *plan);

#ifdef __cplusplus
}
#endif

#endif
