/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/window_catalogue.c
 *
 * PURPOSE:
 *   Implement the window catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/window_catalogue.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool contains_ignore_case(const char *text,const char *query)
{
    const char *candidate;
    size_t query_length;
    size_t index;
    if (text == NULL || query == NULL || query[0] == '\0') return query != NULL && query[0] == '\0';
    query_length = strlen(query);
    for (candidate = text; *candidate != '\0'; ++candidate) {
        for (index = 0U; index < query_length; ++index) {
            if (candidate[index] == '\0' || tolower((unsigned char)candidate[index]) != tolower((unsigned char)query[index])) break;
        }
        if (index == query_length) return true;
    }
    return false;
}

static size_t open_instance_count(
    const UmiUiWorkspaceLayout *layout,
    const char *tool_id)
{
    size_t index;
    size_t count = 0U;

    if (layout == NULL || tool_id == NULL) return 0U;
    for (index = 0U; index < layout->window_count; ++index) {
        if (strcmp(layout->windows[index].tool_id, tool_id) == 0) {
            count += 1U;
        }
    }
    return count;
}

static const UmiUiWindowRecentEntry *recent_entry(
    const UmiUiWindowCatalogue *catalogue,
    const char *tool_id)
{
    size_t index;

    if (catalogue == NULL || tool_id == NULL) return NULL;
    for (index = 0U; index < catalogue->recent_count; ++index) {
        if (strcmp(catalogue->recent[index].tool_id, tool_id) == 0) {
            return &catalogue->recent[index];
        }
    }
    return NULL;
}

static int compare_alphabetical(const void *left, const void *right)
{
    const UmiUiWindowCatalogueItem *left_item =
        (const UmiUiWindowCatalogueItem *)left;
    const UmiUiWindowCatalogueItem *right_item =
        (const UmiUiWindowCatalogueItem *)right;
    return strcmp(left_item->descriptor->title, right_item->descriptor->title);
}

static int compare_recent(const void *left, const void *right)
{
    const UmiUiWindowCatalogueItem *left_item =
        (const UmiUiWindowCatalogueItem *)left;
    const UmiUiWindowCatalogueItem *right_item =
        (const UmiUiWindowCatalogueItem *)right;

    if (left_item->last_opened_at_ms != right_item->last_opened_at_ms) {
        return left_item->last_opened_at_ms > right_item->last_opened_at_ms
            ? -1
            : 1;
    }
    if (left_item->open_count != right_item->open_count) {
        return left_item->open_count > right_item->open_count ? -1 : 1;
    }
    return compare_alphabetical(left, right);
}

static int compare_recommended(const void *left, const void *right)
{
    const UmiUiWindowCatalogueItem *left_item =
        (const UmiUiWindowCatalogueItem *)left;
    const UmiUiWindowCatalogueItem *right_item =
        (const UmiUiWindowCatalogueItem *)right;

    if (left_item->can_open != right_item->can_open) {
        return left_item->can_open ? -1 : 1;
    }
    if (left_item->recent != right_item->recent) {
        return left_item->recent ? -1 : 1;
    }
    return compare_recent(left, right);
}

void umi_ui_window_catalogue_init(UmiUiWindowCatalogue *catalogue)
{
    if (catalogue != NULL) {
        (void)memset(catalogue, 0, sizeof(*catalogue));
        catalogue->revision = 1U;
    }
}

UmiStatus umi_ui_window_catalogue_register(UmiUiWindowCatalogue *catalogue,const UmiUiWindowDescriptor *descriptor)
{
    size_t index;
    if (catalogue == NULL || descriptor == NULL || descriptor->tool_id[0] == '\0' || descriptor->title[0] == '\0' || descriptor->category < UMI_UI_WINDOW_CATEGORY_DEVELOPMENT || descriptor->category > UMI_UI_WINDOW_CATEGORY_GENERAL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].tool_id,descriptor->tool_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_UI_WINDOW_CATALOGUE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *descriptor; catalogue->revision += 1U; return UMI_STATUS_OK;
}
const UmiUiWindowDescriptor *umi_ui_window_catalogue_find(const UmiUiWindowCatalogue *catalogue,const char *tool_id) { size_t index; if (catalogue == NULL || tool_id == NULL) return NULL; for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].tool_id,tool_id) == 0) return &catalogue->items[index]; return NULL; }
size_t umi_ui_window_catalogue_search(const UmiUiWindowCatalogue *catalogue,const char *query,UmiUiWindowCategory category,const UmiUiWindowDescriptor **out_items,size_t capacity)
{
    size_t index; size_t found = 0U;
    if (catalogue == NULL || query == NULL || out_items == NULL) return 0U;
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiUiWindowDescriptor *item = &catalogue->items[index];
        bool category_matches = category == 0 || item->category == category;
        bool text_matches = query[0] == '\0' || contains_ignore_case(item->title,query) || contains_ignore_case(item->description,query) || contains_ignore_case(item->tool_id,query);
        if (category_matches && text_matches) { if (found < capacity) out_items[found] = item; found += 1U; }
    }
    return found;
}

UmiUiWindowCatalogueQuery umi_ui_window_catalogue_query_default(void)
{
    UmiUiWindowCatalogueQuery query;

    (void)memset(&query, 0, sizeof(query));
    query.order = UMI_UI_WINDOW_CATALOGUE_ORDER_RECOMMENDED;
    query.include_singletons_already_open = true;
    query.limit = UMI_UI_WINDOW_CATALOGUE_MAX;
    return query;
}

UmiStatus umi_ui_window_catalogue_query(
    const UmiUiWindowCatalogue *catalogue,
    const UmiUiWorkspaceLayout *active_layout,
    const UmiUiWindowCatalogueQuery *query,
    UmiUiWindowCatalogueResult *out_result)
{
    size_t index;
    size_t limit;

    if (catalogue == NULL || query == NULL || out_result == NULL ||
        query->order < UMI_UI_WINDOW_CATALOGUE_ORDER_RECOMMENDED ||
        query->order > UMI_UI_WINDOW_CATALOGUE_ORDER_RECENT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    limit = query->limit == 0U || query->limit > UMI_UI_WINDOW_CATALOGUE_MAX
        ? UMI_UI_WINDOW_CATALOGUE_MAX
        : query->limit;

    for (index = 0U; index < catalogue->count; ++index) {
        const UmiUiWindowDescriptor *descriptor = &catalogue->items[index];
        const UmiUiWindowRecentEntry *recent =
            recent_entry(catalogue, descriptor->tool_id);
        size_t instances = open_instance_count(
            active_layout, descriptor->tool_id);
        bool category_matches =
            query->category == 0 || descriptor->category == query->category;
        bool mask_matches =
            query->category_mask == 0U ||
            (query->category_mask &
             (UINT32_C(1) << (unsigned int)descriptor->category)) != 0U;
        bool text_matches =
            query->text[0] == '\0' ||
            contains_ignore_case(descriptor->title, query->text) ||
            contains_ignore_case(descriptor->description, query->text) ||
            contains_ignore_case(descriptor->tool_id, query->text);
        bool can_open = descriptor->supports_multiple || instances == 0U;
        UmiUiWindowCatalogueItem *item;

        if (!category_matches || !mask_matches || !text_matches) continue;
        if (!query->include_singletons_already_open && !can_open) continue;

        out_result->total_available += 1U;
        if (out_result->count >= UMI_UI_WINDOW_CATALOGUE_MAX) {
            out_result->truncated = true;
            continue;
        }
        item = &out_result->items[out_result->count++];
        item->descriptor = descriptor;
        item->open_instance_count = instances;
        item->can_open = can_open;
        if (recent != NULL) {
            item->last_opened_at_ms = recent->last_opened_at_ms;
            item->open_count = recent->open_count;
            item->recent = true;
        }
    }

    qsort(
        out_result->items,
        out_result->count,
        sizeof(out_result->items[0]),
        query->order == UMI_UI_WINDOW_CATALOGUE_ORDER_ALPHABETICAL
            ? compare_alphabetical
            : (query->order == UMI_UI_WINDOW_CATALOGUE_ORDER_RECENT
                ? compare_recent
                : compare_recommended));

    if (out_result->count > limit) {
        out_result->count = limit;
        out_result->truncated = true;
    }
    out_result->revision = catalogue->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_window_catalogue_record_open(
    UmiUiWindowCatalogue *catalogue,
    const char *tool_id,
    uint64_t opened_at_ms)
{
    size_t index;
    size_t target = UMI_UI_WINDOW_RECENT_MAX;

    if (catalogue == NULL || tool_id == NULL || tool_id[0] == '\0' ||
        umi_ui_window_catalogue_find(catalogue, tool_id) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < catalogue->recent_count; ++index) {
        if (strcmp(catalogue->recent[index].tool_id, tool_id) == 0) {
            target = index;
            break;
        }
    }
    if (target == UMI_UI_WINDOW_RECENT_MAX) {
        if (catalogue->recent_count < UMI_UI_WINDOW_RECENT_MAX) {
            target = catalogue->recent_count++;
        } else {
            target = 0U;
            for (index = 1U; index < catalogue->recent_count; ++index) {
                if (catalogue->recent[index].last_opened_at_ms <
                    catalogue->recent[target].last_opened_at_ms) {
                    target = index;
                }
            }
        }
        (void)memset(&catalogue->recent[target], 0,
                     sizeof(catalogue->recent[target]));
        (void)snprintf(
            catalogue->recent[target].tool_id,
            sizeof(catalogue->recent[target].tool_id),
            "%s",
            tool_id);
    }

    catalogue->recent[target].last_opened_at_ms = opened_at_ms;
    catalogue->recent[target].open_count += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

const char *umi_ui_window_category_text(UmiUiWindowCategory category)
{
    switch (category) {
        case UMI_UI_WINDOW_CATEGORY_DEVELOPMENT: return "Development";
        case UMI_UI_WINDOW_CATEGORY_NAVIGATION: return "Navigation";
        case UMI_UI_WINDOW_CATEGORY_OPERATIONS: return "Operations";
        case UMI_UI_WINDOW_CATEGORY_DATA: return "Data";
        case UMI_UI_WINDOW_CATEGORY_AI: return "AI";
        case UMI_UI_WINDOW_CATEGORY_TRADING: return "Trading";
        case UMI_UI_WINDOW_CATEGORY_GENERAL: return "General";
        default: return "Unknown";
    }
}
