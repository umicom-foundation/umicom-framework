/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/directory_compare.c
 *
 * PURPOSE:
 *   Implement bounded directory-comparison result aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/developer_productivity/directory_compare.h"

#include <string.h>

/*
 * Initialise developer directory compare from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_directory_compare_init(
    UmiDeveloperDirectoryCompareModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->revision = 1U;
}

/*
 * Add developer directory compare only after its inputs and available capacity have been
 * checked.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_developer_directory_compare_add(
    UmiDeveloperDirectoryCompareModel *model,
    const UmiDeveloperDirectoryCompareItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item == NULL ||
        item->relative_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->item_count >= UMI_DEVELOPER_DIRECTORY_COMPARE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    model->items[model->item_count++] = *item;

    /* Select the behaviour associated with the requested command or state value. */
    switch (item->state) {
        case UMI_DEVELOPER_DIRECTORY_SAME:
            model->same_count += 1U;
            break;
        case UMI_DEVELOPER_DIRECTORY_LEFT_ONLY:
            model->left_only_count += 1U;
            break;
        case UMI_DEVELOPER_DIRECTORY_RIGHT_ONLY:
            model->right_only_count += 1U;
            break;
        case UMI_DEVELOPER_DIRECTORY_DIFFERENT:
        case UMI_DEVELOPER_DIRECTORY_TYPE_MISMATCH:
            model->different_count += 1U;
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    model->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_directory_compare_add(
    UmiDeveloperDirectoryCompareModel *model,
    const UmiDeveloperDirectoryCompareItem *item)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (model != NULL && model->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item == NULL ||
        item->relative_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->item_count >= UMI_DEVELOPER_DIRECTORY_COMPARE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    model->items[model->item_count++] = *item;

    /* Select the behaviour associated with the requested command or state value. */
    switch (item->state) {
        case UMI_DEVELOPER_DIRECTORY_SAME:
            model->same_count += 1U;
            break;
        case UMI_DEVELOPER_DIRECTORY_LEFT_ONLY:
            model->left_only_count += 1U;
            break;
        case UMI_DEVELOPER_DIRECTORY_RIGHT_ONLY:
            model->right_only_count += 1U;
            break;
        case UMI_DEVELOPER_DIRECTORY_DIFFERENT:
        case UMI_DEVELOPER_DIRECTORY_TYPE_MISMATCH:
            model->different_count += 1U;
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    model->revision += 1U;
    return UMI_STATUS_OK;
}
