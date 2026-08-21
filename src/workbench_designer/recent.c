/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/recent.c
 *
 * PURPOSE:
 *   Implement stable recent-layout ordering while preserving pinned entries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/recent.h"
#include "internal.h"


void umi_workbench_designer_recent_init(UmiWorkbenchDesignerRecentList *list)
{
    if (list == NULL) return;
    (void)memset(list, 0, sizeof(*list));
}

UmiStatus umi_workbench_designer_recent_touch(
    UmiWorkbenchDesignerRecentList *list,
    const char *layout_id,
    const char *name,
    uint64_t opened_at_ms,
    uint64_t revision)
{
    UmiWorkbenchDesignerRecentItem item;
    size_t index;
    size_t existing = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    if (list == NULL || layout_id == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&item, 0, sizeof(item));
    (void)umi_workbench_designer_copy_text(item.layout_id, sizeof(item.layout_id), layout_id);
    (void)umi_workbench_designer_copy_text(item.name, sizeof(item.name), name);
    item.opened_at_ms = opened_at_ms;
    item.revision = revision;
    for (index = 0U; index < list->count; ++index) {
        if (strcmp(list->items[index].layout_id, layout_id) == 0) {
            item.pinned = list->items[index].pinned;
            existing = index;
            break;
        }
    }
    if (existing != UMI_WORKBENCH_DESIGNER_INDEX_NONE) {
        for (index = existing + 1U; index < list->count; ++index) {
            list->items[index - 1U] = list->items[index];
        }
        list->count -= 1U;
    } else if (list->count == 32U) {
        size_t remove_index = list->count - 1U;
        while (remove_index > 0U && list->items[remove_index].pinned) remove_index -= 1U;
        for (index = remove_index + 1U; index < list->count; ++index) {
            list->items[index - 1U] = list->items[index];
        }
        list->count -= 1U;
    }
    for (index = list->count; index > 0U; --index) {
        list->items[index] = list->items[index - 1U];
    }
    list->items[0] = item;
    list->count += 1U;
    list->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_recent_pin(
    UmiWorkbenchDesignerRecentList *list,
    const char *layout_id,
    bool pinned)
{
    size_t index;
    if (list == NULL || layout_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < list->count; ++index) {
        if (strcmp(list->items[index].layout_id, layout_id) == 0) {
            list->items[index].pinned = pinned;
            list->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiWorkbenchDesignerRecentItem *umi_workbench_designer_recent_at(
    const UmiWorkbenchDesignerRecentList *list,
    size_t index)
{
    if (list == NULL || index >= list->count) return NULL;
    return &list->items[index];
}
