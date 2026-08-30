/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_directory_model.c
 *
 * PURPOSE:
 *   Verify folder-compare aggregation, filtering and selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/directory_model.h"

int main(void)
{
    UmiVcsWorkbenchDirectoryModel model;
    UmiVcsAdvancedDirectoryEntry left;
    UmiVcsAdvancedDirectoryEntry right;
    UmiVcsAdvancedDirectoryFilter filter;
    umi_vcs_workbench_directory_model_init(&model);
    umi_vcs_advanced_directory_entry_init(&left);
    umi_vcs_advanced_directory_entry_init(&right);
    assert(umi_vcs_advanced_directory_entry_set(
               &left, "src/a.c", 10U, 100U, 0) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_directory_entry_set(
               &right, "src/a.c", 12U, 101U, 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_directory_model_add(&model, &left, &right) ==
           UMI_STATUS_OK);
    assert(model.changed_count == 1U);
    assert(model.visible_count == 1U);
    umi_vcs_advanced_directory_filter_init(&filter);
    assert(umi_vcs_workbench_directory_model_set_filter(
               &model, &filter, 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_directory_model_select(&model, 0U) ==
           UMI_STATUS_OK);
    assert(umi_vcs_workbench_directory_model_visible_at(&model, 0U) != NULL);
    return 0;
}
