/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/explorer_model.h
 *
 * PURPOSE:
 *   Build a deterministic, toolkit-neutral Test Explorer tree from canonical
 *   test items and retained results. The model owns filtering, sorting,
 *   expansion, selection and visible-row projection for every frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named test explorer sort mode values accepted by this public contract.
 */
typedef enum UmiTestExplorerSortMode {
    UMI_TEST_EXPLORER_SORT_NAME = 1,
    UMI_TEST_EXPLORER_SORT_SOURCE = 2,
    UMI_TEST_EXPLORER_SORT_OUTCOME = 3,
    UMI_TEST_EXPLORER_SORT_DURATION = 4,
    UMI_TEST_EXPLORER_SORT_FRAMEWORK = 5
} UmiTestExplorerSortMode;

/**
 * List the named test explorer group mode values accepted by this public contract.
 */
typedef enum UmiTestExplorerGroupMode {
    UMI_TEST_EXPLORER_GROUP_HIERARCHY = 1,
    UMI_TEST_EXPLORER_GROUP_SUITE = 2,
    UMI_TEST_EXPLORER_GROUP_FRAMEWORK = 3,
    UMI_TEST_EXPLORER_GROUP_OUTCOME = 4
} UmiTestExplorerGroupMode;

/**
 * Represent the test explorer row data shared with callers of this public contract.
 */
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

/**
 * Represent the test explorer model snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the test explorer model data shared with callers of this public contract.
 */
typedef struct UmiTestExplorerModel UmiTestExplorerModel;

/**
 * Initialise test explorer model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_explorer_model_create(UmiTestExplorerModel **out_model);
/**
 * Release or reset state held by test explorer model so the same storage can be reused
 * safely.
 */
void umi_test_explorer_model_destroy(UmiTestExplorerModel *model);
/**
 * Provide the test explorer model set filter operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_set_filter(
    UmiTestExplorerModel *model,
    const UmiTestPlatformFilter *filter);
/**
 * Provide the test explorer model set sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_set_sort(
    UmiTestExplorerModel *model,
    UmiTestExplorerSortMode sort_mode);
/**
 * Provide the test explorer model set group operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_set_group(
    UmiTestExplorerModel *model,
    UmiTestExplorerGroupMode group_mode);
/**
 * Provide the test explorer model rebuild operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_rebuild(
    UmiTestExplorerModel *model,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results);
/**
 * Provide the test explorer model set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_model_set_expanded(
    UmiTestExplorerModel *model,
    const char *item_id,
    int expanded);
/**
 * Provide the test explorer model expand all operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_expand_all(UmiTestExplorerModel *model);
/**
 * Provide the test explorer model collapse all operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_model_collapse_all(UmiTestExplorerModel *model);
/**
 * Provide the test explorer model select operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_select(
    UmiTestExplorerModel *model,
    const char *item_id,
    int selected,
    int exclusive);
/**
 * Provide the test explorer model clear selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_model_clear_selection(UmiTestExplorerModel *model);
/**
 * Provide the test explorer model selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_selection(
    const UmiTestExplorerModel *model,
    UmiTestPlatformSelection *out_selection);
/**
 * Find test explorer model row while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_explorer_model_row_at(
    const UmiTestExplorerModel *model,
    size_t position,
    UmiTestExplorerRow *out_row);
/**
 * Find test explorer model visible row while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_explorer_model_visible_row_at(
    const UmiTestExplorerModel *model,
    size_t visible_position,
    UmiTestExplorerRow *out_row);
/**
 * Provide the test explorer model snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_model_snapshot(
    const UmiTestExplorerModel *model,
    UmiTestExplorerModelSnapshot *out_snapshot);
/**
 * Return the number of records represented by test explorer model without changing their
 * state.
 */
size_t umi_test_explorer_model_count(const UmiTestExplorerModel *model);
/**
 * Return the number of records represented by test explorer model visible without changing
 * their state.
 */
size_t umi_test_explorer_model_visible_count(const UmiTestExplorerModel *model);
/**
 * Provide the test explorer model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_test_explorer_model_revision(const UmiTestExplorerModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_EXPLORER_MODEL_H */
