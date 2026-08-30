/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/window_catalogue_v2.c
 *
 * PURPOSE:
 *   Implement the window catalogue v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace window catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/window_catalogue_v2.h"
#include <ctype.h>
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

UmiStatus umi_ui_window_catalogue_v2_register(UmiUiWindowCatalogueV2 *catalogue,const UmiUiWindowDescriptor *descriptor)
{
    size_t index;
    if (catalogue == NULL || descriptor == NULL || descriptor->tool_id[0] == '\0' || descriptor->title[0] == '\0' || descriptor->category < UMI_UI_WINDOW_CATEGORY_DEVELOPMENT || descriptor->category > UMI_UI_WINDOW_CATEGORY_GENERAL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].tool_id,descriptor->tool_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_UI_WINDOW_CATALOGUE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *descriptor; catalogue->revision += 1U; return UMI_STATUS_OK;
}
const UmiUiWindowDescriptor *umi_ui_window_catalogue_v2_find(const UmiUiWindowCatalogueV2 *catalogue,const char *tool_id) { size_t index; if (catalogue == NULL || tool_id == NULL) return NULL; for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].tool_id,tool_id) == 0) return &catalogue->items[index]; return NULL; }
size_t umi_ui_window_catalogue_v2_search(const UmiUiWindowCatalogueV2 *catalogue,const char *query,UmiUiWindowCategory category,const UmiUiWindowDescriptor **out_items,size_t capacity)
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
