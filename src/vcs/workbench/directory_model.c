/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/directory_model.c
 *
 * PURPOSE:
 *   Implement filtering, selection and summaries over canonical directory
 *   comparison entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/vcs/workbench/directory_model.h"

#include <string.h>

/* Provide the entry visible operation used by this module and its client applications. */
static int entry_visible(
    const UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryDiff *entry)
{
    const UmiVcsAdvancedDirectoryEntry *sample =
        entry->has_left ? &entry->left : &entry->right;
    const char *slash;
    int hidden;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!model->show_equal && entry->state == UMI_VCS_DIRECTORY_EQUAL) {
        return 0;
    }
    slash = strrchr(entry->relative_path, '/');
    hidden = (slash != NULL ? slash + 1 : entry->relative_path)[0] == '.';
    return umi_vcs_advanced_directory_filter_accept(
        &model->filter, entry->relative_path, sample->size_bytes,
        sample->directory, hidden, 0);
}

/* Provide the rebuild visible operation used by this module and its client applications. */
static void rebuild_visible(UmiVcsWorkbenchDirectoryModel *model)
{
    size_t index;

    model->visible_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (entry_visible(model, &model->entries[index])) {
            model->visible_indices[model->visible_count++] = index;
        }
    }
    /* Apply this operation only while the related capability or state is available. */
    if (model->visible_count == 0U) {
        model->selected_visible_index = 0U;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (model->selected_visible_index >= model->visible_count) {
        model->selected_visible_index = model->visible_count - 1U;
    }
}

/*
 * Initialise vcs workbench directory model from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_workbench_directory_model_init(
    UmiVcsWorkbenchDirectoryModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    model->show_equal = 1;
    model->ready = 1;
    model->revision = 1U;
    umi_vcs_advanced_directory_filter_init(&model->filter);
}

/*
 * Add vcs workbench directory model only after its inputs and available capacity have been
 * checked.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_vcs_workbench_directory_model_add(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryEntry *left,
    const UmiVcsAdvancedDirectoryEntry *right)
{
    UmiVcsAdvancedDirectoryDiff *entry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_VCS_WORKBENCH_MAX_DIRECTORY_ENTRIES) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &model->entries[model->count];
    status = umi_vcs_advanced_directory_diff_compare(left, right, entry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->count += 1U;
    /* Select the behaviour associated with the requested command or state value. */
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
#endif

UmiStatus umi_vcs_workbench_directory_model_add(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryEntry *left,
    const UmiVcsAdvancedDirectoryEntry *right)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (model != NULL && model->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiVcsAdvancedDirectoryDiff *entry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_VCS_WORKBENCH_MAX_DIRECTORY_ENTRIES) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &model->entries[model->count];
    status = umi_vcs_advanced_directory_diff_compare(left, right, entry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->count += 1U;
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the vcs workbench directory model set filter operation used by this module and
 * its client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_vcs_workbench_directory_model_set_filter(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryFilter *filter,
    int show_equal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
#endif

UmiStatus umi_vcs_workbench_directory_model_set_filter(
    UmiVcsWorkbenchDirectoryModel *model,
    const UmiVcsAdvancedDirectoryFilter *filter,
    int show_equal)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (model != NULL && model->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the vcs workbench directory model select operation used by this module and its
 * client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_vcs_workbench_directory_model_select(
    UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (visible_index >= model->visible_count) return UMI_STATUS_NOT_FOUND;
    model->selected_visible_index = visible_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_vcs_workbench_directory_model_select(
    UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (model != NULL && model->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (visible_index >= model->visible_count) return UMI_STATUS_NOT_FOUND;
    model->selected_visible_index = visible_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workbench directory model visible while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiVcsAdvancedDirectoryDiff *
umi_vcs_workbench_directory_model_visible_at(
    const UmiVcsWorkbenchDirectoryModel *model,
    size_t visible_index)
{
    return model != NULL && visible_index < model->visible_count
        ? &model->entries[model->visible_indices[visible_index]] : NULL;
}
