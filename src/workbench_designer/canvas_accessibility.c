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


void umi_workbench_designer_accessibility_init(
    UmiWorkbenchDesignerAccessibilityModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

static UmiWorkbenchDesignerAccessibleRole accessibility_role(
    UmiWorkbenchLayoutNodeKind kind)
{
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

static size_t accessibility_parent(
    const UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchDesignerTreeRow *row)
{
    size_t index;
    if (tree == NULL || row == NULL || row->depth == 0U) {
        return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    }
    index = (size_t)(row - tree->rows);
    while (index > 0U) {
        index -= 1U;
        if (tree->rows[index].depth + 1U == row->depth) return index;
    }
    return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

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
    if (model == NULL || canvas == NULL || tree == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_accessibility_init(model);
    for (index = 0U; index < tree->row_count; ++index) {
        const UmiWorkbenchDesignerTreeRow *row = &tree->rows[index];
        const UmiWorkbenchDesignerCanvasItem *canvas_item =
            umi_workbench_designer_canvas_find(canvas, row->node_id);
        UmiWorkbenchDesignerAccessibleItem *item;
        int written;
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
        if (!layout_locked && canvas_item != NULL) {
            item->action_mask |= UMI_WORKBENCH_DESIGNER_ACCESSIBLE_MOVE |
                UMI_WORKBENCH_DESIGNER_ACCESSIBLE_RESIZE;
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

const UmiWorkbenchDesignerAccessibleItem *umi_workbench_designer_accessibility_at(
    const UmiWorkbenchDesignerAccessibilityModel *model,
    size_t index)
{
    return model != NULL && index < model->count ? &model->items[index] : NULL;
}

const UmiWorkbenchDesignerAccessibleItem *umi_workbench_designer_accessibility_find(
    const UmiWorkbenchDesignerAccessibilityModel *model,
    const char *node_id)
{
    size_t index;
    if (model == NULL || node_id == NULL) return NULL;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].node_id, node_id) == 0) {
            return &model->items[index];
        }
    }
    return NULL;
}
