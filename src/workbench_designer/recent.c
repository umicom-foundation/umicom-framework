/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/recent.c
 *
 * PURPOSE:
 *   Implement stable recent-layout ordering while preserving pinned entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/recent.h"
#include "internal.h"


/*
 * Initialise workbench designer recent from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_recent_init(UmiWorkbenchDesignerRecentList *list)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return;
    (void)memset(list, 0, sizeof(*list));
}

/*
 * Provide the workbench designer recent touch operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || layout_id == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&item, 0, sizeof(item));
    (void)umi_workbench_designer_copy_text(item.layout_id, sizeof(item.layout_id), layout_id);
    (void)umi_workbench_designer_copy_text(item.name, sizeof(item.name), name);
    item.opened_at_ms = opened_at_ms;
    item.revision = revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(list->items[index].layout_id, layout_id) == 0) {
            item.pinned = list->items[index].pinned;
            existing = index;
            break;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (existing != UMI_WORKBENCH_DESIGNER_INDEX_NONE) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = existing + 1U; index < list->count; ++index) {
            list->items[index - 1U] = list->items[index];
        }
        list->count -= 1U;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (list->count == 32U) {
        size_t remove_index = list->count - 1U;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (remove_index > 0U && list->items[remove_index].pinned) remove_index -= 1U;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = remove_index + 1U; index < list->count; ++index) {
            list->items[index - 1U] = list->items[index];
        }
        list->count -= 1U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = list->count; index > 0U; --index) {
        list->items[index] = list->items[index - 1U];
    }
    list->items[0] = item;
    list->count += 1U;
    list->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer recent pin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_recent_pin(
    UmiWorkbenchDesignerRecentList *list,
    const char *layout_id,
    bool pinned)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || layout_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(list->items[index].layout_id, layout_id) == 0) {
            list->items[index].pinned = pinned;
            list->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find workbench designer recent while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerRecentItem *umi_workbench_designer_recent_at(
    const UmiWorkbenchDesignerRecentList *list,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || index >= list->count) return NULL;
    return &list->items[index];
}
