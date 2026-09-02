/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/canvas_accessibility.c
 *
 * PURPOSE:
 *   Map semantic layout nodes to frontend accessibility roles and permitted
 *   actions without depending on GTK accessibility objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/canvas_accessibility.h"
#include "internal.h"
#include <stdio.h>


/*
 * Initialise workbench designer accessibility from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_accessibility_init(
    UmiWorkbenchDesignerAccessibilityModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

/*
 * Provide the accessibility role operation used by this module and its client
 * applications.
 */
static UmiWorkbenchDesignerAccessibleRole accessibility_role(
    UmiWorkbenchLayoutNodeKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP:
            return UMI_WORKBENCH_DESIGNER_ACCESSIBLE_TAB_LIST;
        case UMI_WORKBENCH_LAYOUT_NODE_PANEL:
            return UMI_WORKBENCH_DESIGNER_ACCESSIBLE_PANEL;
        case UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP:
            return UMI_WORKBENCH_DESIGNER_ACCESSIBLE_EDITOR;
        case UMI_WORKBENCH_LAYOUT_NODE_WINDOW:
        case UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW:
            return UMI_WORKBENCH_DESIGNER_ACCESSIBLE_WINDOW;
        case UMI_WORKBENCH_LAYOUT_NODE_SPLIT:
            return UMI_WORKBENCH_DESIGNER_ACCESSIBLE_GROUP;
        default:
            return UMI_WORKBENCH_DESIGNER_ACCESSIBLE_CANVAS;
    }
}

/*
 * Provide the accessibility parent operation used by this module and its client
 * applications.
 */
static size_t accessibility_parent(
    const UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchDesignerTreeRow *row)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || row == NULL || row->depth == 0U) {
        return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    }
    index = (size_t)(row - tree->rows);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        index -= 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (tree->rows[index].depth + 1U == row->depth) return index;
    }
    return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

/*
 * Provide the workbench designer accessibility build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_accessibility_build(
    UmiWorkbenchDesignerAccessibilityModel *model,
    const UmiWorkbenchDesignerCanvas *canvas,
    const UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchDesignerFocusRing *focus_ring,
    bool layout_locked)
{
    const UmiWorkbenchDesignerFocusItem *focused =
        umi_workbench_designer_focus_ring_current(focus_ring);
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || canvas == NULL || tree == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_accessibility_init(model);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tree->row_count; ++index) {
        const UmiWorkbenchDesignerTreeRow *row = &tree->rows[index];
        const UmiWorkbenchDesignerCanvasItem *canvas_item =
            umi_workbench_designer_canvas_find(canvas, row->node_id);
        UmiWorkbenchDesignerAccessibleItem *item;
        int written;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        item = &model->items[model->count++];
        (void)memset(item, 0, sizeof(*item));
        (void)umi_workbench_designer_copy_text(
            item->node_id, sizeof(item->node_id), row->node_id);
        (void)umi_workbench_designer_copy_text(
            item->name, sizeof(item->name), row->title);
        written = snprintf(
            item->description, sizeof(item->description),
            "%s layout node at depth %zu%s.",
            umi_workbench_layout_node_kind_text(row->kind), row->depth,
            row->selected ? ", selected" : "");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(item->description)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        item->role = accessibility_role(row->kind);
        item->parent_index = accessibility_parent(tree, row);
        item->depth = row->depth;
        item->selected = row->selected;
        item->focused = focused != NULL &&
            strcmp(focused->node_id, row->node_id) == 0;
        item->hidden = !row->visible;
        item->locked = layout_locked;
        item->action_mask = UMI_WORKBENCH_DESIGNER_ACCESSIBLE_FOCUS |
            UMI_WORKBENCH_DESIGNER_ACCESSIBLE_SELECT |
            UMI_WORKBENCH_DESIGNER_ACCESSIBLE_OPEN;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (!layout_locked && canvas_item != NULL) {
            item->action_mask |= UMI_WORKBENCH_DESIGNER_ACCESSIBLE_MOVE |
                UMI_WORKBENCH_DESIGNER_ACCESSIBLE_RESIZE;
            /* Apply this branch only when its contract condition is satisfied. */
            if (!canvas_item->container) {
                item->action_mask |= UMI_WORKBENCH_DESIGNER_ACCESSIBLE_CLOSE;
            }
        }
    }
    model->canvas_revision = canvas->revision;
    model->tree_revision = tree->revision;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer accessibility while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerAccessibleItem *umi_workbench_designer_accessibility_at(
    const UmiWorkbenchDesignerAccessibilityModel *model,
    size_t index)
{
    return model != NULL && index < model->count ? &model->items[index] : NULL;
}

/*
 * Find workbench designer accessibility while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerAccessibleItem *umi_workbench_designer_accessibility_find(
    const UmiWorkbenchDesignerAccessibilityModel *model,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].node_id, node_id) == 0) {
            return &model->items[index];
        }
    }
    return NULL;
}
