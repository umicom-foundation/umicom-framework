/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/directory_compare.h
 *
 * PURPOSE:
 *   Model directory-comparison results independently from the filesystem walker
 *   so local, remote and archive providers can share the same UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIRECTORY_COMPARE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIRECTORY_COMPARE_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_DIRECTORY_COMPARE_CAPACITY 256U

typedef enum UmiDeveloperDirectoryCompareState {
    UMI_DEVELOPER_DIRECTORY_SAME = 0,
    UMI_DEVELOPER_DIRECTORY_LEFT_ONLY = 1,
    UMI_DEVELOPER_DIRECTORY_RIGHT_ONLY = 2,
    UMI_DEVELOPER_DIRECTORY_DIFFERENT = 3,
    UMI_DEVELOPER_DIRECTORY_TYPE_MISMATCH = 4
} UmiDeveloperDirectoryCompareState;

typedef struct UmiDeveloperDirectoryCompareItem {
    char relative_path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    UmiDeveloperDirectoryCompareState state;
    uint64_t left_size;
    uint64_t right_size;
    int left_directory;
    int right_directory;
} UmiDeveloperDirectoryCompareItem;

typedef struct UmiDeveloperDirectoryCompareModel {
    UmiDeveloperDirectoryCompareItem
        items[UMI_DEVELOPER_DIRECTORY_COMPARE_CAPACITY];
    size_t item_count;
    size_t same_count;
    size_t different_count;
    size_t left_only_count;
    size_t right_only_count;
    uint64_t revision;
} UmiDeveloperDirectoryCompareModel;

void umi_developer_directory_compare_init(
    UmiDeveloperDirectoryCompareModel *model);

UmiStatus umi_developer_directory_compare_add(
    UmiDeveloperDirectoryCompareModel *model,
    const UmiDeveloperDirectoryCompareItem *item);

#ifdef __cplusplus
}
#endif

#endif
