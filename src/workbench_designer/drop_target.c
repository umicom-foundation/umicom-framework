/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/drop_target.c
 *
 * PURPOSE:
 *   Create left, right, top, bottom, centre and tab targets from semantic canvas
 *   geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/drop_target.h"
#include "internal.h"
#include <stdio.h>


/*
 * Initialise workbench designer drop targets from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_drop_targets_init(
    UmiWorkbenchDesignerDropTargetSet *targets)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (targets == NULL) return;
    (void)memset(targets, 0, sizeof(*targets));
}

/* Add drop target only after its inputs and available capacity have been checked. */
static UmiStatus drop_target_add(
    UmiWorkbenchDesignerDropTargetSet *targets,
    const UmiWorkbenchDesignerCanvasItem *item,
    UmiWorkbenchDesignerDropZone zone,
    UmiWorkbenchDesignerRect bounds,
    UmiWorkbenchDesignerRect preview,
    int32_t priority)
{
    UmiWorkbenchDesignerDropTarget *target;
    int written;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (targets->count >= UMI_WORKBENCH_DESIGNER_MAX_DROP_TARGETS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    target = &targets->targets[targets->count++];
    (void)memset(target, 0, sizeof(*target));
    written = snprintf(target->target_id, sizeof(target->target_id), "%s:%s",
        item->node_id, umi_workbench_designer_drop_zone_text(zone));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(target->target_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_workbench_designer_copy_text(
        target->target_node_id, sizeof(target->target_node_id), item->node_id);
    target->zone = zone;
    target->bounds = bounds;
    target->preview_bounds = preview;
    target->depth = item->depth;
    target->priority = priority;
    target->enabled = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer drop targets build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_drop_targets_build(
    UmiWorkbenchDesignerDropTargetSet *targets,
    const UmiWorkbenchDesignerCanvas *canvas,
    const char *dragged_node_id)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (targets == NULL || canvas == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_drop_targets_init(targets);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        UmiWorkbenchDesignerRect bounds;
        UmiWorkbenchDesignerRect preview;
        double edge_width;
        double edge_height;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (!item->visible || (dragged_node_id != NULL &&
            strcmp(item->node_id, dragged_node_id) == 0) ||
            item->screen_bounds.width <= 0.0 || item->screen_bounds.height <= 0.0) {
            continue;
        }
        edge_width = item->screen_bounds.width * 0.25;
        edge_height = item->screen_bounds.height * 0.25;
        bounds = item->screen_bounds;
        bounds.width = edge_width;
        preview = item->world_bounds;
        preview.width *= 0.5;
        status = drop_target_add(targets, item, UMI_WORKBENCH_DESIGNER_DROP_LEFT, bounds, preview, 50);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        bounds.x = item->screen_bounds.x + item->screen_bounds.width - edge_width;
        preview.x = item->world_bounds.x + item->world_bounds.width * 0.5;
        status = drop_target_add(targets, item, UMI_WORKBENCH_DESIGNER_DROP_RIGHT, bounds, preview, 50);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        bounds = item->screen_bounds;
        bounds.height = edge_height;
        preview = item->world_bounds;
        preview.height *= 0.5;
        status = drop_target_add(targets, item, UMI_WORKBENCH_DESIGNER_DROP_TOP, bounds, preview, 50);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        bounds.y = item->screen_bounds.y + item->screen_bounds.height - edge_height;
        preview.y = item->world_bounds.y + item->world_bounds.height * 0.5;
        status = drop_target_add(targets, item, UMI_WORKBENCH_DESIGNER_DROP_BOTTOM, bounds, preview, 50);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        bounds = item->screen_bounds;
        bounds.x += edge_width;
        bounds.y += edge_height;
        bounds.width -= edge_width * 2.0;
        bounds.height -= edge_height * 2.0;
        preview = item->world_bounds;
        status = drop_target_add(
            targets, item,
            item->kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP
                ? UMI_WORKBENCH_DESIGNER_DROP_TAB
                : UMI_WORKBENCH_DESIGNER_DROP_CENTRE,
            bounds, preview, 100);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    targets->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer drop target while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerDropTarget *umi_workbench_designer_drop_target_at(
    const UmiWorkbenchDesignerDropTargetSet *targets,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (targets == NULL || index >= targets->count) return NULL;
    return &targets->targets[index];
}

/*
 * Provide the workbench designer drop target hit test operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDropTarget *umi_workbench_designer_drop_target_hit_test(
    const UmiWorkbenchDesignerDropTargetSet *targets,
    UmiWorkbenchDesignerPoint point)
{
    const UmiWorkbenchDesignerDropTarget *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (targets == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < targets->count; ++index) {
        const UmiWorkbenchDesignerDropTarget *target = &targets->targets[index];
        /* Apply this operation only while the related capability or state is available. */
        if (!target->enabled || !umi_workbench_designer_rect_contains_point(&target->bounds, point)) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || target->priority > best->priority ||
            (target->priority == best->priority && target->depth > best->depth)) {
            best = target;
        }
    }
    return best;
}
