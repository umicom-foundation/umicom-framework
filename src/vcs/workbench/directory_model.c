/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/directory_model.c
 *
 * PURPOSE:
 *   Implement filtering, selection and summaries over canonical directory
 *   comparison entries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/directory_model.h"

#include <string.h>

static int entry_visible(
    const UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryDiff *entry)
{
    const UmiVcsAdvancedDirectoryEntry *sample =
        entry->has_left ? &entry->left : &entry->right;
    const char *slash;
    int hidden;

    if (!model->show_equal && entry->state == UMI_VCS_DIRECTORY_EQUAL) {
        return 0;
    }
    slash = strrchr(entry->relative_path, '/');
    hidden = (slash != NULL ? slash + 1 : entry->relative_path)[0] == '.';
    return umi_vcs_advanced_directory_filter_accept(
        &model->filter, entry->relative_path, sample->size_bytes,
        sample->directory, hidden, 0);
}

static void rebuild_visible(UmiVcsWorkbenchDirectoryModel *model)
{
    size_t index;

    model->visible_count = 0U;
    for (index = 0U; index < model->count; ++index) {
        if (entry_visible(model, &model->entries[index])) {
            model->visible_indices[model->visible_count++] = index;
        }
    }
    if (model->visible_count == 0U) {
        model->selected_visible_index = 0U;
    } else if (model->selected_visible_index >= model->visible_count) {
        model->selected_visible_index = model->visible_count - 1U;
    }
}

void umi_vcs_workbench_directory_model_init(
    UmiVcsWorkbenchDirectoryModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    model->show_equal = 1;
    model->ready = 1;
    model->revision = 1U;
    umi_vcs_advanced_directory_filter_init(&model->filter);
}

UmiStatus umi_vcs_workbench_directory_model_add(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryEntry *left,
    const UmiVcsAdvancedDirectoryEntry *right)
{
    UmiVcsAdvancedDirectoryDiff *entry;
    UmiStatus status;

    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count >= UMI_VCS_WORKBENCH_MAX_DIRECTORY_ENTRIES) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &model->entries[model->count];
    status = umi_vcs_advanced_directory_diff_compare(left, right, entry);
    if (status != UMI_STATUS_OK) return status;
    model->count += 1U;
    switch (entry->state) {
        case UMI_VCS_DIRECTORY_EQUAL: model->equal_count += 1U; break;
        case UMI_VCS_DIRECTORY_LEFT_ONLY: model->left_only_count += 1U; break;
        case UMI_VCS_DIRECTORY_RIGHT_ONLY: model->right_only_count += 1U; break;
        case UMI_VCS_DIRECTORY_DIFFERENT: model->changed_count += 1U; break;
        case UMI_VCS_DIRECTORY_TYPE_CHANGED:
            model->type_changed_count += 1U;
            break;
        default: break;
    }
    rebuild_visible(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_directory_model_set_filter(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryFilter *filter,
    int show_equal)
{
    if (model == NULL ||
        umi_vcs_advanced_directory_filter_validate(filter) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->filter = *filter;
    model->show_equal = show_equal != 0;
    rebuild_visible(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_directory_model_select(
    UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (visible_index >= model->visible_count) return UMI_STATUS_NOT_FOUND;
    model->selected_visible_index = visible_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiVcsAdvancedDirectoryDiff *
umi_vcs_workbench_directory_model_visible_at(
    const UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index)
{
    return model != NULL && visible_index < model->visible_count
        ? &model->entries[model->visible_indices[visible_index]] : NULL;
}
