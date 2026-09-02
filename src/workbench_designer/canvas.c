/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/canvas.c
 *
 * PURPOSE:
 *   Compute a frontend-neutral visual tree so GTK4, Qt, Wt and headless tests
 *   can render identical semantic layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/canvas.h"
#include "internal.h"
#include <float.h>


/*
 * Provide the canvas screen rect operation used by this module and its client
 * applications.
 */
static UmiWorkbenchDesignerRect canvas_screen_rect(
    const UmiWorkbenchDesignerViewport *viewport,
    UmiWorkbenchDesignerRect world)
{
    UmiWorkbenchDesignerPoint top_left = {world.x, world.y};
    UmiWorkbenchDesignerPoint bottom_right = {
        world.x + world.width,
        world.y + world.height};
    UmiWorkbenchDesignerPoint screen_top_left =
        umi_workbench_designer_viewport_world_to_screen(viewport, top_left);
    UmiWorkbenchDesignerPoint screen_bottom_right =
        umi_workbench_designer_viewport_world_to_screen(viewport, bottom_right);
    UmiWorkbenchDesignerRect result;
    result.x = screen_top_left.x;
    result.y = screen_top_left.y;
    result.width = screen_bottom_right.x - screen_top_left.x;
    result.height = screen_bottom_right.y - screen_top_left.y;
    return result;
}

/*
 * Provide the canvas append item operation used by this module and its client
 * applications.
 */
static UmiStatus canvas_append_item(
    UmiWorkbenchDesignerCanvas *canvas,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerViewport *viewport,
    const UmiWorkbenchDesignerSelection *selection,
    size_t node_index,
    size_t depth,
    UmiWorkbenchDesignerRect bounds)
{
    const UmiWorkbenchLayoutNode *node;
    UmiWorkbenchDesignerCanvasItem *item;
    const char *primary;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (canvas->count >= UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_document_index_valid(document, node_index)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = &document->nodes[node_index];
    item = &canvas->items[canvas->count++];
    (void)memset(item, 0, sizeof(*item));
    (void)umi_workbench_designer_copy_text(
        item->node_id, sizeof(item->node_id), node->node_id);
    (void)umi_workbench_designer_copy_text(
        item->title, sizeof(item->title), node->title);
    (void)umi_workbench_designer_copy_text(
        item->component_id, sizeof(item->component_id), node->component_id);
    item->kind = node->kind;
    item->world_bounds = bounds;
    item->screen_bounds = canvas_screen_rect(viewport, bounds);
    item->node_index = node_index;
    item->depth = depth;
    item->z_order = node->z_order;
    item->visible = node->visibility != UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    item->container = umi_workbench_layout_node_is_container(node);
    item->selected = selection != NULL &&
        umi_workbench_designer_selection_contains(selection, node->node_id);
    primary = selection != NULL
        ? umi_workbench_designer_selection_primary(selection) : NULL;
    item->primary = primary != NULL && strcmp(primary, node->node_id) == 0;
    return UMI_STATUS_OK;
}

/* Provide the canvas visit operation used by this module and its client applications. */
static UmiStatus canvas_visit(
    UmiWorkbenchDesignerCanvas *canvas,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerViewport *viewport,
    const UmiWorkbenchDesignerSelection *selection,
    size_t node_index,
    size_t depth,
    UmiWorkbenchDesignerRect bounds,
    bool *visited)
{
    const UmiWorkbenchLayoutNode *node;
    size_t child_position;
    double cursor;
    UmiStatus status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (depth > UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        !umi_workbench_designer_document_index_valid(document, node_index)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (visited[node_index]) return UMI_STATUS_INVALID_STATE;
    visited[node_index] = true;
    status = canvas_append_item(
        canvas, document, viewport, selection, node_index, depth, bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    node = &document->nodes[node_index];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (node->child_count == 0U) return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT) {
        cursor = node->orientation == UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL
            ? bounds.x : bounds.y;
        /* Visit each bounded item once so every record receives the same rule. */
        for (child_position = 0U;
             child_position < node->child_count;
             ++child_position) {
            UmiWorkbenchDesignerRect child_bounds = bounds;
            double share;
            /* Apply this branch only when its contract condition is satisfied. */
            if (node->child_count == 2U) {
                share = child_position == 0U
                    ? node->split_ratio : 1.0 - node->split_ratio;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                share = 1.0 / (double)node->child_count;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (node->orientation == UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL) {
                child_bounds.x = cursor;
                child_bounds.width = bounds.width * share;
                cursor += child_bounds.width;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                child_bounds.y = cursor;
                child_bounds.height = bounds.height * share;
                cursor += child_bounds.height;
            }
            status = canvas_visit(
                canvas, document, viewport, selection,
                node->child_indices[child_position], depth + 1U,
                child_bounds, visited);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
        return UMI_STATUS_OK;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (child_position = 0U;
         child_position < node->child_count;
         ++child_position) {
        UmiWorkbenchDesignerRect child_bounds = bounds;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP &&
            child_position != node->active_child_index) {
            child_bounds.width = 0.0;
            child_bounds.height = 0.0;
        }
        status = canvas_visit(
            canvas, document, viewport, selection,
            node->child_indices[child_position], depth + 1U,
            child_bounds, visited);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench designer canvas from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_canvas_init(UmiWorkbenchDesignerCanvas *canvas)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL) return;
    (void)memset(canvas, 0, sizeof(*canvas));
}

/*
 * Provide the workbench designer canvas build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_canvas_build(
    UmiWorkbenchDesignerCanvas *canvas,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerViewport *viewport,
    const UmiWorkbenchDesignerSelection *selection,
    UmiWorkbenchDesignerRect root_bounds)
{
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || document == NULL || viewport == NULL ||
        !umi_workbench_designer_rect_is_valid(&root_bounds) ||
        document->root_index >= document->node_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_canvas_init(canvas);
    (void)memset(visited, 0, sizeof(visited));
    canvas->world_bounds = root_bounds;
    status = canvas_visit(
        canvas, document, viewport, selection,
        document->root_index, 0U, root_bounds, visited);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    canvas->document_revision = document->version.revision;
    canvas->viewport_revision = viewport->revision;
    canvas->selection_revision = selection != NULL ? selection->revision : 0U;
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer canvas while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_at(
    const UmiWorkbenchDesignerCanvas *canvas,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || index >= canvas->count) return NULL;
    return &canvas->items[index];
}

/*
 * Find workbench designer canvas while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_find(
    const UmiWorkbenchDesignerCanvas *canvas,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || node_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(canvas->items[index].node_id, node_id) == 0) {
            return &canvas->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench designer canvas hit test operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_canvas_hit_test(
    const UmiWorkbenchDesignerCanvas *canvas,
    UmiWorkbenchDesignerPoint screen_point,
    bool include_containers)
{
    const UmiWorkbenchDesignerCanvasItem *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (!item->visible || (!include_containers && item->container)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_workbench_designer_rect_contains_point(
                &item->screen_bounds, screen_point)) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (best == NULL || item->depth > best->depth ||
                (item->depth == best->depth && item->z_order > best->z_order)) {
                best = item;
            }
        }
    }
    return best;
}

/*
 * Provide the workbench designer canvas selection bounds operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerRect umi_workbench_designer_canvas_selection_bounds(
    const UmiWorkbenchDesignerCanvas *canvas)
{
    UmiWorkbenchDesignerRect result = {0.0, 0.0, 0.0, 0.0};
    double min_x = DBL_MAX;
    double min_y = DBL_MAX;
    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;
    size_t index;
    bool found = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL) return result;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!item->selected) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->world_bounds.x < min_x) min_x = item->world_bounds.x;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->world_bounds.y < min_y) min_y = item->world_bounds.y;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->world_bounds.x + item->world_bounds.width > max_x) {
            max_x = item->world_bounds.x + item->world_bounds.width;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->world_bounds.y + item->world_bounds.height > max_y) {
            max_y = item->world_bounds.y + item->world_bounds.height;
        }
        found = true;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (found) {
        result.x = min_x;
        result.y = min_y;
        result.width = max_x - min_x;
        result.height = max_y - min_y;
    }
    return result;
}
