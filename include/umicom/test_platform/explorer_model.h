/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/explorer_model.h
 *
 * PURPOSE:
 *   Build a deterministic, toolkit-neutral Test Explorer tree from canonical
 *   test items and retained results. The model owns filtering, sorting,
 *   expansion, selection and visible-row projection for every frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_EXPLORER_MODEL_H
#define UMICOM_TEST_PLATFORM_EXPLORER_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/test_platform/filter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_EXPLORER_MODEL_API_VERSION 1U
#define UMI_TEST_EXPLORER_MODEL_CAPACITY 4096U

typedef enum UmiTestExplorerSortMode {
    UMI_TEST_EXPLORER_SORT_NAME = 1,
    UMI_TEST_EXPLORER_SORT_SOURCE = 2,
    UMI_TEST_EXPLORER_SORT_OUTCOME = 3,
    UMI_TEST_EXPLORER_SORT_DURATION = 4,
    UMI_TEST_EXPLORER_SORT_FRAMEWORK = 5
} UmiTestExplorerSortMode;

typedef enum UmiTestExplorerGroupMode {
    UMI_TEST_EXPLORER_GROUP_HIERARCHY = 1,
    UMI_TEST_EXPLORER_GROUP_SUITE = 2,
    UMI_TEST_EXPLORER_GROUP_FRAMEWORK = 3,
    UMI_TEST_EXPLORER_GROUP_OUTCOME = 4
} UmiTestExplorerGroupMode;

typedef struct UmiTestExplorerRow {
    uint32_t struct_size;
    uint32_t api_version;
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot latest_result;
    size_t depth;
    size_t child_count;
    size_t visible_index;
    int has_result;
    int expanded;
    int selected;
    int visible;
    int matches_filter;
    int descendant_matches_filter;
} UmiTestExplorerRow;

typedef struct UmiTestExplorerModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiTestExplorerSortMode sort_mode;
    UmiTestExplorerGroupMode group_mode;
    UmiTestPlatformFilter filter;
    size_t row_count;
    size_t visible_row_count;
    size_t root_count;
    size_t leaf_count;
    size_t selected_count;
    size_t passed_count;
    size_t failed_count;
    size_t skipped_count;
    size_t not_run_count;
    uint64_t item_revision;
    uint64_t result_revision;
    uint64_t revision;
} UmiTestExplorerModelSnapshot;

typedef struct UmiTestExplorerModel UmiTestExplorerModel;

UmiStatus umi_test_explorer_model_create(UmiTestExplorerModel **out_model);
void umi_test_explorer_model_destroy(UmiTestExplorerModel *model);
UmiStatus umi_test_explorer_model_set_filter(
    UmiTestExplorerModel *model,
    const UmiTestPlatformFilter *filter);
UmiStatus umi_test_explorer_model_set_sort(
    UmiTestExplorerModel *model,
    UmiTestExplorerSortMode sort_mode);
UmiStatus umi_test_explorer_model_set_group(
    UmiTestExplorerModel *model,
    UmiTestExplorerGroupMode group_mode);
UmiStatus umi_test_explorer_model_rebuild(
    UmiTestExplorerModel *model,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results);
UmiStatus umi_test_explorer_model_set_expanded(
    UmiTestExplorerModel *model,
    const char *item_id,
    int expanded);
UmiStatus umi_test_explorer_model_expand_all(UmiTestExplorerModel *model);
UmiStatus umi_test_explorer_model_collapse_all(UmiTestExplorerModel *model);
UmiStatus umi_test_explorer_model_select(
    UmiTestExplorerModel *model,
    const char *item_id,
    int selected,
    int exclusive);
UmiStatus umi_test_explorer_model_clear_selection(UmiTestExplorerModel *model);
UmiStatus umi_test_explorer_model_selection(
    const UmiTestExplorerModel *model,
    UmiTestPlatformSelection *out_selection);
UmiStatus umi_test_explorer_model_row_at(
    const UmiTestExplorerModel *model,
    size_t position,
    UmiTestExplorerRow *out_row);
UmiStatus umi_test_explorer_model_visible_row_at(
    const UmiTestExplorerModel *model,
    size_t visible_position,
    UmiTestExplorerRow *out_row);
UmiStatus umi_test_explorer_model_snapshot(
    const UmiTestExplorerModel *model,
    UmiTestExplorerModelSnapshot *out_snapshot);
size_t umi_test_explorer_model_count(const UmiTestExplorerModel *model);
size_t umi_test_explorer_model_visible_count(const UmiTestExplorerModel *model);
uint64_t umi_test_explorer_model_revision(const UmiTestExplorerModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_EXPLORER_MODEL_H */
