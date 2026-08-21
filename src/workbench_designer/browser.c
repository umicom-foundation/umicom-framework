/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/browser.c
 *
 * PURPOSE:
 *   Implement Layout Browser filtering and stable ordering over persistent
 *   layout summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/browser.h"
#include "internal.h"
#include <stdlib.h>


void umi_workbench_designer_browser_init(UmiWorkbenchDesignerBrowser *browser)
{
    if (browser == NULL) return;
    (void)memset(browser, 0, sizeof(*browser));
    browser->sort = UMI_WORKBENCH_DESIGNER_BROWSER_SORT_NAME;
}

static bool browser_item_matches(
    const UmiWorkbenchDesignerBrowserItem *item,
    const UmiWorkbenchDesignerBrowserQuery *query)
{
    if (query->category[0] != '\0' && strcmp(item->category, query->category) != 0) return false;
    if (query->owner[0] != '\0' && strcmp(item->owner, query->owner) != 0) return false;
    if (query->workspace[0] != '\0' && strcmp(item->workspace, query->workspace) != 0) return false;
    if (query->tag[0] != '\0' && !umi_workbench_designer_text_contains_case_insensitive(item->tags, query->tag)) return false;
    if (query->built_in_only && !item->built_in) return false;
    if (query->user_only && item->built_in) return false;
    if (query->locked_only && !item->locked) return false;
    if (query->shared_only && !item->shared) return false;
    if (query->text[0] != '\0' &&
        !umi_workbench_designer_text_contains_case_insensitive(item->name, query->text) &&
        !umi_workbench_designer_text_contains_case_insensitive(item->description, query->text) &&
        !umi_workbench_designer_text_contains_case_insensitive(item->tags, query->text) &&
        !umi_workbench_designer_text_contains_case_insensitive(item->layout_id, query->text)) return false;
    return true;
}

static const UmiWorkbenchDesignerBrowser *browser_sort_context;

static int browser_compare_indices(const void *left_pointer, const void *right_pointer)
{
    size_t left_index = *(const size_t *)left_pointer;
    size_t right_index = *(const size_t *)right_pointer;
    const UmiWorkbenchDesignerBrowserItem *left = &browser_sort_context->items[left_index];
    const UmiWorkbenchDesignerBrowserItem *right = &browser_sort_context->items[right_index];
    int comparison = 0;
    switch (browser_sort_context->sort) {
        case UMI_WORKBENCH_DESIGNER_BROWSER_SORT_RECENT:
            comparison = left->modified_at_ms < right->modified_at_ms ? -1 :
                left->modified_at_ms > right->modified_at_ms ? 1 : 0;
            break;
        case UMI_WORKBENCH_DESIGNER_BROWSER_SORT_REVISION:
            comparison = left->revision < right->revision ? -1 :
                left->revision > right->revision ? 1 : 0;
            break;
        case UMI_WORKBENCH_DESIGNER_BROWSER_SORT_OWNER:
            comparison = strcmp(left->owner, right->owner);
            break;
        case UMI_WORKBENCH_DESIGNER_BROWSER_SORT_NAME:
        default:
            comparison = strcmp(left->name, right->name);
            break;
    }
    if (comparison == 0) comparison = strcmp(left->layout_id, right->layout_id);
    return browser_sort_context->descending ? -comparison : comparison;
}

static void browser_refresh(UmiWorkbenchDesignerBrowser *browser)
{
    size_t index;
    browser->visible_count = 0U;
    for (index = 0U; index < browser->count; ++index) {
        if (browser_item_matches(&browser->items[index], &browser->query)) {
            browser->visible_indices[browser->visible_count++] = index;
        }
    }
    browser_sort_context = browser;
    qsort(browser->visible_indices, browser->visible_count,
          sizeof(browser->visible_indices[0]), browser_compare_indices);
    browser_sort_context = NULL;
    browser->revision += 1U;
}

UmiStatus umi_workbench_designer_browser_add_summary(
    UmiWorkbenchDesignerBrowser *browser,
    const UmiWorkbenchLayoutRecordSummary *summary,
    const char *description,
    const char *tags,
    bool active)
{
    UmiWorkbenchDesignerBrowserItem *item;
    if (browser == NULL || summary == NULL || summary->layout_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (browser->count >= UMI_WORKBENCH_DESIGNER_MAX_BROWSER_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item = &browser->items[browser->count++];
    (void)memset(item, 0, sizeof(*item));
    (void)umi_workbench_designer_copy_text(item->layout_id, sizeof(item->layout_id), summary->layout_id);
    (void)umi_workbench_designer_copy_text(item->name, sizeof(item->name), summary->name);
    (void)umi_workbench_designer_copy_text(item->category, sizeof(item->category), summary->category);
    (void)umi_workbench_designer_copy_text(item->owner, sizeof(item->owner), summary->owner_user_id);
    (void)umi_workbench_designer_copy_text(item->owner_application_id, sizeof(item->owner_application_id), summary->owner_application_id);
    (void)umi_workbench_designer_copy_text(item->workspace, sizeof(item->workspace), summary->workspace_id);
    (void)umi_workbench_designer_copy_text(item->description, sizeof(item->description), description != NULL ? description : "");
    (void)umi_workbench_designer_copy_text(item->tags, sizeof(item->tags), tags != NULL ? tags : "");
    item->revision = summary->revision;
    item->modified_at_ms = summary->modified_at_ms;
    item->content_hash = summary->content_hash;
    item->built_in = (summary->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN) != 0U;
    item->locked = (summary->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED) != 0U;
    item->shared = (summary->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED) != 0U;
    item->active = active;
    browser_refresh(browser);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_browser_set_query(
    UmiWorkbenchDesignerBrowser *browser,
    const UmiWorkbenchDesignerBrowserQuery *query)
{
    if (browser == NULL || query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    browser->query = *query;
    browser_refresh(browser);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_browser_set_sort(
    UmiWorkbenchDesignerBrowser *browser,
    UmiWorkbenchDesignerBrowserSort sort,
    bool descending)
{
    if (browser == NULL || sort < UMI_WORKBENCH_DESIGNER_BROWSER_SORT_NAME ||
        sort > UMI_WORKBENCH_DESIGNER_BROWSER_SORT_OWNER) return UMI_STATUS_INVALID_ARGUMENT;
    browser->sort = sort;
    browser->descending = descending;
    browser_refresh(browser);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_browser_select(
    UmiWorkbenchDesignerBrowser *browser,
    const char *layout_id)
{
    size_t index;
    bool found = false;
    if (browser == NULL || layout_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < browser->count; ++index) {
        browser->items[index].selected = strcmp(browser->items[index].layout_id, layout_id) == 0;
        if (browser->items[index].selected) found = true;
    }
    if (!found) return UMI_STATUS_NOT_FOUND;
    (void)umi_workbench_designer_copy_text(
        browser->selected_layout_id, sizeof(browser->selected_layout_id), layout_id);
    browser->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerBrowserItem *umi_workbench_designer_browser_visible_at(
    const UmiWorkbenchDesignerBrowser *browser,
    size_t index)
{
    if (browser == NULL || index >= browser->visible_count) return NULL;
    return &browser->items[browser->visible_indices[index]];
}

const UmiWorkbenchDesignerBrowserItem *umi_workbench_designer_browser_selected(
    const UmiWorkbenchDesignerBrowser *browser)
{
    size_t index;
    if (browser == NULL) return NULL;
    for (index = 0U; index < browser->count; ++index) {
        if (browser->items[index].selected) return &browser->items[index];
    }
    return NULL;
}
