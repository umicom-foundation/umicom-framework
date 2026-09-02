/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/diff_view.c
 *
 * PURPOSE:
 *   Build stable review rows from the existing semantic diff engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/diff_view.h"
#include "internal.h"
#include <stdio.h>


/*
 * Initialise workbench designer diff view from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_diff_view_init(UmiWorkbenchDesignerDiffView *view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return;
    (void)memset(view, 0, sizeof(*view));
    view->selected_index = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    view->geometry_visible = true;
}

/*
 * Provide the workbench designer diff view build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_diff_view_build(
    UmiWorkbenchDesignerDiffView *view,
    const UmiWorkbenchLayoutDiff *diff)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || diff == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_diff_view_init(view);
    /* Apply this branch only when its contract condition is satisfied. */
    if (diff->entry_count > UMI_WORKBENCH_LAYOUT_MAX_DIFF_ENTRIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < diff->entry_count; ++index) {
        const UmiWorkbenchLayoutDiffEntry *entry = &diff->entries[index];
        UmiWorkbenchDesignerDiffRow *row = &view->rows[view->count++];
        int written;
        (void)memset(row, 0, sizeof(*row));
        written = snprintf(row->row_id, sizeof(row->row_id), "diff-%zu", index);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(row->row_id)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)umi_workbench_designer_copy_text(row->node_id, sizeof(row->node_id), entry->node_id);
        (void)umi_workbench_designer_copy_text(row->field, sizeof(row->field), entry->field);
        (void)umi_workbench_designer_copy_text(row->before_value, sizeof(row->before_value), entry->before_value);
        (void)umi_workbench_designer_copy_text(row->after_value, sizeof(row->after_value), entry->after_value);
        row->kind = entry->kind;
        row->side = entry->kind == UMI_WORKBENCH_LAYOUT_DIFF_NODE_ADDED
            ? UMI_WORKBENCH_DESIGNER_DIFF_AFTER_ONLY
            : entry->kind == UMI_WORKBENCH_LAYOUT_DIFF_NODE_REMOVED
                ? UMI_WORKBENCH_DESIGNER_DIFF_BEFORE_ONLY
                : UMI_WORKBENCH_DESIGNER_DIFF_BOTH;
        written = snprintf(row->summary, sizeof(row->summary), "%s: %s",
            umi_workbench_layout_diff_kind_text(entry->kind),
            entry->node_id[0] != '\0' ? entry->node_id : entry->field);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(row->summary)) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    view->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer diff view select operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_diff_view_select(
    UmiWorkbenchDesignerDiffView *view,
    size_t index)
{
    size_t row_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || index >= view->count) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (row_index = 0U; row_index < view->count; ++row_index) {
        view->rows[row_index].selected = row_index == index;
    }
    view->selected_index = index;
    view->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer diff view while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerDiffRow *umi_workbench_designer_diff_view_at(
    const UmiWorkbenchDesignerDiffView *view,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || index >= view->count) return NULL;
    return &view->rows[index];
}
