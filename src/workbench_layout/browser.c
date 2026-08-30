/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/browser.c
 *
 * PURPOSE:
 *   Implement Layout Browser filtering, access projection, pagination and deterministic sorting over persisted semantic layout metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/browser.h"

#include <string.h>

#include "internal.h"

static int compare_items(
    const UmiWorkbenchLayoutBrowserItem *left,
    const UmiWorkbenchLayoutBrowserItem *right,
    UmiWorkbenchLayoutSortOrder order)
{
    int result;

    switch (order) {
    case UMI_WORKBENCH_LAYOUT_SORT_NAME_DESCENDING:
        return -strcmp(left->summary.name, right->summary.name);
    case UMI_WORKBENCH_LAYOUT_SORT_RECENT_FIRST:
        if (left->summary.modified_at_ms >
            right->summary.modified_at_ms) {
            return -1;
        }
        if (left->summary.modified_at_ms <
            right->summary.modified_at_ms) {
            return 1;
        }
        break;
    case UMI_WORKBENCH_LAYOUT_SORT_REVISION_DESCENDING:
        if (left->summary.revision >
            right->summary.revision) {
            return -1;
        }
        if (left->summary.revision <
            right->summary.revision) {
            return 1;
        }
        break;
    case UMI_WORKBENCH_LAYOUT_SORT_NAME_ASCENDING:
    default:
        break;
    }

    result = strcmp(left->summary.name, right->summary.name);
    if (result != 0) {
        return result;
    }
    return strcmp(
        left->summary.layout_id,
        right->summary.layout_id);
}

UmiWorkbenchLayoutBrowserQuery
umi_workbench_layout_browser_query_default(void)
{
    UmiWorkbenchLayoutBrowserQuery query;

    (void)memset(&query, 0, sizeof(query));
    query.structure_size = sizeof(query);
    query.sort_order =
        UMI_WORKBENCH_LAYOUT_SORT_NAME_ASCENDING;
    query.include_built_in = true;
    query.include_shared = true;
    query.include_read_only = true;
    query.offset = 0U;
    query.limit =
        UMI_WORKBENCH_LAYOUT_MAX_BROWSER_RESULTS;
    return query;
}

UmiStatus umi_workbench_layout_browser_sort(
    UmiWorkbenchLayoutBrowserResult *result,
    UmiWorkbenchLayoutSortOrder order)
{
    size_t index;
    size_t position;

    if (result == NULL ||
        order < UMI_WORKBENCH_LAYOUT_SORT_NAME_ASCENDING ||
        order >
            UMI_WORKBENCH_LAYOUT_SORT_REVISION_DESCENDING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 1U; index < result->count; ++index) {
        UmiWorkbenchLayoutBrowserItem selected =
            result->items[index];
        position = index;
        while (position > 0U &&
               compare_items(
                   &selected,
                   &result->items[position - 1U],
                   order) < 0) {
            result->items[position] =
                result->items[position - 1U];
            position -= 1U;
        }
        result->items[position] = selected;
    }
    result->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_browser_search(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutBrowserQuery *query,
    const char *current_user_id,
    const char *active_layout_id,
    UmiWorkbenchLayoutBrowserResult *out_result)
{
    UmiWorkbenchLayoutBrowserQuery effective;
    UmiWorkbenchLayoutStoreQuery store_query;
    UmiWorkbenchLayoutStoreList list;
    size_t index;
    UmiStatus status;

    if (adapter == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    effective = query != NULL
        ? *query
        : umi_workbench_layout_browser_query_default();
    store_query =
        umi_workbench_layout_store_query_default();
    (void)umi_workbench_layout_copy_text(
        store_query.owner_user_id,
        sizeof(store_query.owner_user_id),
        effective.owner_user_id,
        true);
    (void)umi_workbench_layout_copy_text(
        store_query.owner_application_id,
        sizeof(store_query.owner_application_id),
        effective.owner_application_id,
        true);
    (void)umi_workbench_layout_copy_text(
        store_query.workspace_id,
        sizeof(store_query.workspace_id),
        effective.workspace_id,
        true);
    (void)umi_workbench_layout_copy_text(
        store_query.category,
        sizeof(store_query.category),
        effective.category,
        true);
    (void)umi_workbench_layout_copy_text(
        store_query.text,
        sizeof(store_query.text),
        effective.text,
        true);
    store_query.include_built_in =
        effective.include_built_in;
    store_query.include_shared =
        effective.include_shared;
    store_query.include_read_only =
        effective.include_read_only;
    store_query.offset = effective.offset;
    store_query.limit = effective.limit;

    status = umi_workbench_layout_store_list(
        adapter, &store_query, &list);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->structure_size = sizeof(*out_result);
    out_result->total_available = list.total_available;
    out_result->truncated = list.truncated;
    out_result->revision = 1U;

    for (index = 0U;
         index < list.count &&
         out_result->count <
             UMI_WORKBENCH_LAYOUT_MAX_BROWSER_RESULTS;
         ++index) {
        UmiWorkbenchLayoutBrowserItem *item =
            &out_result->items[out_result->count];
        bool owner =
            current_user_id != NULL &&
            current_user_id[0] != '\0' &&
            strcmp(
                current_user_id,
                list.records[index].owner_user_id) == 0;
        bool read_only =
            (list.records[index].flags &
             (uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY) != 0U;
        bool built_in =
            (list.records[index].flags &
             (uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN) != 0U;

        (void)memset(item, 0, sizeof(*item));
        item->structure_size = sizeof(*item);
        item->summary = list.records[index];
        item->owned_by_current_user = owner;
        item->editable = owner && !read_only;
        item->deletable = owner && !read_only && !built_in;
        item->shareable = owner || !read_only;
        item->active =
            active_layout_id != NULL &&
            strcmp(
                active_layout_id,
                item->summary.layout_id) == 0;
        out_result->count += 1U;
    }

    return umi_workbench_layout_browser_sort(
        out_result, effective.sort_order);
}

const UmiWorkbenchLayoutBrowserItem *
umi_workbench_layout_browser_result_at(
    const UmiWorkbenchLayoutBrowserResult *result,
    size_t index)
{
    if (result == NULL || index >= result->count) {
        return NULL;
    }
    return &result->items[index];
}
