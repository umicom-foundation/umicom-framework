/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/directory_model.h
 *
 * PURPOSE:
 *   Aggregate canonical directory-diff records into a filterable, selectable
 *   folder-comparison workbench model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_DIRECTORY_MODEL_H
#define UMICOM_VCS_WORKBENCH_DIRECTORY_MODEL_H

#include "umicom/vcs/advanced/directory_diff.h"
#include "umicom/vcs/advanced/directory_filter.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs workbench directory model data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkbenchDirectoryModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedDirectoryDiff entries[
        UMI_VCS_WORKBENCH_MAX_DIRECTORY_ENTRIES];
    size_t visible_indices[UMI_VCS_WORKBENCH_MAX_DIRECTORY_ENTRIES];
    size_t count;
    size_t visible_count;
    size_t selected_visible_index;
    size_t equal_count;
    size_t changed_count;
    size_t left_only_count;
    size_t right_only_count;
    size_t type_changed_count;
    UmiVcsAdvancedDirectoryFilter filter;
    int show_equal;
    int ready;
    int truncated;
    uint64_t revision;
} UmiVcsWorkbenchDirectoryModel;

/**
 * Initialise vcs workbench directory model from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_workbench_directory_model_init(
    UmiVcsWorkbenchDirectoryModel *model);
/**
 * Add vcs workbench directory model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_workbench_directory_model_add(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryEntry *left,
    const UmiVcsAdvancedDirectoryEntry *right);
/**
 * Provide the vcs workbench directory model set filter operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_directory_model_set_filter(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryFilter *filter,
    int show_equal);
/**
 * Provide the vcs workbench directory model select operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_directory_model_select(
    UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index);
/**
 * Find vcs workbench directory model visible while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiVcsAdvancedDirectoryDiff *
umi_vcs_workbench_directory_model_visible_at(
    const UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_DIRECTORY_MODEL_H */
