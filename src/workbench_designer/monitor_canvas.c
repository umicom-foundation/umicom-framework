/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/monitor_canvas.c
 *
 * PURPOSE:
 *   Implement multi-monitor placement and relocation without operating-system
 *   window handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/monitor_canvas.h"
#include "internal.h"
#include <float.h>


/*
 * Initialise workbench designer monitor canvas from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_monitor_canvas_init(
    UmiWorkbenchDesignerMonitorCanvas *canvas)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL) return;
    (void)memset(canvas, 0, sizeof(*canvas));
}

/*
 * Find workbench designer monitor canvas while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_find(
    const UmiWorkbenchDesignerMonitorCanvas *canvas,
    const char *monitor_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || monitor_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(canvas->monitors[index].monitor_id, monitor_id) == 0) {
            return &canvas->monitors[index];
        }
    }
    return NULL;
}

/*
 * Provide the monitor canvas refresh bounds operation used by this module and its client
 * applications.
 */
static void monitor_canvas_refresh_bounds(UmiWorkbenchDesignerMonitorCanvas *canvas)
{
    double min_x = DBL_MAX;
    double min_y = DBL_MAX;
    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;
    size_t index;
    bool found = false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerMonitor *monitor = &canvas->monitors[index];
        /* Apply this operation only while the related capability or state is available. */
        if (!monitor->enabled) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor->bounds.x < min_x) min_x = monitor->bounds.x;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor->bounds.y < min_y) min_y = monitor->bounds.y;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor->bounds.x + monitor->bounds.width > max_x) max_x = monitor->bounds.x + monitor->bounds.width;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor->bounds.y + monitor->bounds.height > max_y) max_y = monitor->bounds.y + monitor->bounds.height;
        found = true;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (found) {
        canvas->combined_bounds.x = min_x;
        canvas->combined_bounds.y = min_y;
        canvas->combined_bounds.width = max_x - min_x;
        canvas->combined_bounds.height = max_y - min_y;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        canvas->combined_bounds = (UmiWorkbenchDesignerRect){0.0, 0.0, 0.0, 0.0};
    }
}

/*
 * Add workbench designer monitor canvas only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_designer_monitor_canvas_add(
    UmiWorkbenchDesignerMonitorCanvas *canvas,
    const UmiWorkbenchDesignerMonitor *monitor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || monitor == NULL || monitor->monitor_id[0] == '\0' ||
        !umi_workbench_designer_rect_is_valid(&monitor->bounds) ||
        !umi_workbench_designer_rect_is_valid(&monitor->work_area) ||
        monitor->scale <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_workbench_designer_monitor_canvas_find(canvas, monitor->monitor_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (canvas->count >= UMI_WORKBENCH_DESIGNER_MAX_MONITORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    canvas->monitors[canvas->count++] = *monitor;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (monitor->primary || canvas->primary_monitor_id[0] == '\0') {
        (void)umi_workbench_designer_copy_text(
            canvas->primary_monitor_id, sizeof(canvas->primary_monitor_id),
            monitor->monitor_id);
    }
    monitor_canvas_refresh_bounds(canvas);
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench designer monitor canvas while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_designer_monitor_canvas_remove(
    UmiWorkbenchDesignerMonitorCanvas *canvas,
    const char *monitor_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || monitor_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(canvas->monitors[index].monitor_id, monitor_id) == 0) {
            size_t move_index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (move_index = index + 1U; move_index < canvas->count; ++move_index) {
                canvas->monitors[move_index - 1U] = canvas->monitors[move_index];
            }
            canvas->count -= 1U;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(canvas->primary_monitor_id, monitor_id) == 0) {
                canvas->primary_monitor_id[0] = '\0';
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (canvas->count > 0U) {
                    (void)umi_workbench_designer_copy_text(
                        canvas->primary_monitor_id,
                        sizeof(canvas->primary_monitor_id),
                        canvas->monitors[0].monitor_id);
                }
            }
            monitor_canvas_refresh_bounds(canvas);
            canvas->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench designer monitor canvas primary operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_primary(
    const UmiWorkbenchDesignerMonitorCanvas *canvas)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL) return NULL;
    return umi_workbench_designer_monitor_canvas_find(
        canvas, canvas->primary_monitor_id);
}

/*
 * Provide the workbench designer monitor canvas clamp operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerRect umi_workbench_designer_monitor_canvas_clamp(
    const UmiWorkbenchDesignerMonitorCanvas *canvas,
    const char *monitor_id,
    UmiWorkbenchDesignerRect bounds)
{
    const UmiWorkbenchDesignerMonitor *monitor =
        umi_workbench_designer_monitor_canvas_find(canvas, monitor_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) monitor = umi_workbench_designer_monitor_canvas_primary(canvas);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) return bounds;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bounds.width > monitor->work_area.width) bounds.width = monitor->work_area.width;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bounds.height > monitor->work_area.height) bounds.height = monitor->work_area.height;
    bounds.x = umi_workbench_designer_clamp(
        bounds.x,
        monitor->work_area.x,
        monitor->work_area.x + monitor->work_area.width - bounds.width);
    bounds.y = umi_workbench_designer_clamp(
        bounds.y,
        monitor->work_area.y,
        monitor->work_area.y + monitor->work_area.height - bounds.height);
    return bounds;
}

/*
 * Provide the workbench designer monitor canvas relocate missing operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_designer_monitor_canvas_relocate_missing(
    UmiWorkbenchDesignerMonitorCanvas *canvas,
    UmiWorkbenchLayoutDocument *document)
{
    const UmiWorkbenchDesignerMonitor *primary;
    size_t index;
    bool changed = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    primary = umi_workbench_designer_monitor_canvas_primary(canvas);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary == NULL) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->dock_region != UMI_WORKBENCH_LAYOUT_DOCK_FLOATING) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (node->monitor_id[0] == '\0' ||
            umi_workbench_designer_monitor_canvas_find(canvas, node->monitor_id) == NULL) {
            UmiWorkbenchDesignerRect bounds =
                umi_workbench_designer_from_layout_rect(node->bounds);
            UmiWorkbenchLayoutRect layout_bounds;
            bounds = umi_workbench_designer_monitor_canvas_clamp(
                canvas, primary->monitor_id, bounds);
            layout_bounds = umi_workbench_designer_to_layout_rect(bounds);
            (void)umi_workbench_layout_node_set_monitor(node, primary->monitor_id);
            (void)umi_workbench_layout_node_set_bounds(node, &layout_bounds);
            node->revision += 1U;
            changed = true;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (changed) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    return UMI_STATUS_OK;
}
