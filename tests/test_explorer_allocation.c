/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_explorer_allocation.c
 * PURPOSE: Verify rebuild refusal preserves model state at each allocation boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/test_platform/explorer_model.h"
static size_t calls, live;
static size_t reject = SIZE_MAX;
static void *ExplorerTestCalloc(size_t count, size_t size)
{
    if (calls++ == reject) return NULL;
    void *value = calloc(count, size);
    if (value != NULL) ++live;
    return value;
}
static void *ExplorerTestRealloc(void *old, size_t size)
{
    if (calls++ == reject) return NULL;
    const int absent = old == NULL;
    void *value = realloc(old, size);
    if (value != NULL && absent) ++live;
    return value;
}
static void ExplorerTestFree(void *value)
{
    if (value != NULL) --live;
    free(value);
}
/* Compile the production implementation with local allocation interception.
 * Renamed public symbols avoid colliding with the linked registry library;
 * no allocation hooks or test-only failure controls enter the product ABI. */
#define umi_test_explorer_model_create ExplorerTest_umi_test_explorer_model_create
#define umi_test_explorer_model_destroy ExplorerTest_umi_test_explorer_model_destroy
#define umi_test_explorer_model_set_filter ExplorerTest_umi_test_explorer_model_set_filter
#define umi_test_explorer_model_set_sort ExplorerTest_umi_test_explorer_model_set_sort
#define umi_test_explorer_model_set_group ExplorerTest_umi_test_explorer_model_set_group
#define umi_test_explorer_model_rebuild ExplorerTest_umi_test_explorer_model_rebuild
#define umi_test_explorer_model_set_expanded ExplorerTest_umi_test_explorer_model_set_expanded
#define umi_test_explorer_model_expand_all ExplorerTest_umi_test_explorer_model_expand_all
#define umi_test_explorer_model_collapse_all ExplorerTest_umi_test_explorer_model_collapse_all
#define umi_test_explorer_model_select ExplorerTest_umi_test_explorer_model_select
#define umi_test_explorer_model_clear_selection ExplorerTest_umi_test_explorer_model_clear_selection
#define umi_test_explorer_model_selection ExplorerTest_umi_test_explorer_model_selection
#define umi_test_explorer_model_row_at ExplorerTest_umi_test_explorer_model_row_at
#define umi_test_explorer_model_visible_row_at ExplorerTest_umi_test_explorer_model_visible_row_at
#define umi_test_explorer_model_snapshot ExplorerTest_umi_test_explorer_model_snapshot
#define umi_test_explorer_model_count ExplorerTest_umi_test_explorer_model_count
#define umi_test_explorer_model_visible_count ExplorerTest_umi_test_explorer_model_visible_count
#define umi_test_explorer_model_revision ExplorerTest_umi_test_explorer_model_revision
#define calloc ExplorerTestCalloc
#define realloc ExplorerTestRealloc
#define free ExplorerTestFree
#include "../src/test_platform/explorer_model.c"
#undef calloc
#undef realloc
#undef free
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(void)
{
    int failed = 0, reached_success = 0;
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestExplorerModel *model = NULL;
    UmiTestExplorerModelSnapshot before, after;
    UmiTestExplorerRow first, retained;
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    UmiTestPlatformItemSnapshot item = {0};
    strcpy(item.id, "original"); strcpy(item.name, "Original"); strcpy(item.kind, "test"); item.enabled = 1;
    CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_create(&model) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_select(model, "original", 1, 1) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_snapshot(model, &before) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_row_at(model, 0U, &first) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 200U; ++i) {
        snprintf(item.id, sizeof item.id, "new.%03zu", i);
        memset(item.command, 'x', sizeof item.command - 1U); item.command[sizeof item.command - 1U] = 0;
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    }
    const size_t baseline_live = live;
    for (size_t point = 0U; point < 100U; ++point) {
        calls = 0U; reject = point;
        const UmiStatus status = umi_test_explorer_model_rebuild(model, items, results);
        reject = SIZE_MAX;
        if (status == UMI_STATUS_OK) { reached_success = 1; break; }
        CHECK(status == UMI_STATUS_OUT_OF_MEMORY && live == baseline_live);
        CHECK(umi_test_explorer_model_snapshot(model, &after) == UMI_STATUS_OK);
        CHECK(memcmp(&before, &after, sizeof before) == 0);
        CHECK(umi_test_explorer_model_row_at(model, 0U, &retained) == UMI_STATUS_OK);
        CHECK(memcmp(&first, &retained, sizeof first) == 0);
    }
    CHECK(reached_success);
    CHECK(umi_test_explorer_model_count(model) == 201U);
    CHECK(umi_test_explorer_model_snapshot(model, &after) == UMI_STATUS_OK && after.selected_count == 1U);
done:
    reject = SIZE_MAX;
    umi_test_explorer_model_destroy(model);
    umi_test_platform_result_registry_destroy(results); umi_test_platform_item_registry_destroy(items);
    if (live != 0U) { fprintf(stderr, "Model allocations retained: %zu\n", live); failed = 1; }
    return failed;
}
