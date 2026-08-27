/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/directory_model.h
 *
 * PURPOSE:
 *   Aggregate canonical directory-diff records into a filterable, selectable
 *   folder-comparison workbench model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_DIRECTORY_MODEL_H
#define UMICOM_VCS_WORKBENCH_DIRECTORY_MODEL_H

#include "umicom/vcs/advanced/directory_diff.h"
#include "umicom/vcs/advanced/directory_filter.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_vcs_workbench_directory_model_init(
    UmiVcsWorkbenchDirectoryModel *model);
UmiStatus umi_vcs_workbench_directory_model_add(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryEntry *left,
    const UmiVcsAdvancedDirectoryEntry *right);
UmiStatus umi_vcs_workbench_directory_model_set_filter(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryFilter *filter,
    int show_equal);
UmiStatus umi_vcs_workbench_directory_model_select(
    UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index);
const UmiVcsAdvancedDirectoryDiff *
umi_vcs_workbench_directory_model_visible_at(
    const UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_DIRECTORY_MODEL_H */
