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
#include "umicom/developer_productivity/directory_compare.h"

#include <string.h>

void umi_developer_directory_compare_init(
    UmiDeveloperDirectoryCompareModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->revision = 1U;
}

UmiStatus umi_developer_directory_compare_add(
    UmiDeveloperDirectoryCompareModel *model,
    const UmiDeveloperDirectoryCompareItem *item)
{
    if (model == NULL || item == NULL ||
        item->relative_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (model->item_count >= UMI_DEVELOPER_DIRECTORY_COMPARE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    model->items[model->item_count++] = *item;

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
