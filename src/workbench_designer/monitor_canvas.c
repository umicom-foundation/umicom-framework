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


void umi_workbench_designer_monitor_canvas_init(
    UmiWorkbenchDesignerMonitorCanvas *canvas)
{
    if (canvas == NULL) return;
    (void)memset(canvas, 0, sizeof(*canvas));
}

const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_find(
    const UmiWorkbenchDesignerMonitorCanvas *canvas,
    const char *monitor_id)
{
    size_t index;
    if (canvas == NULL || monitor_id == NULL) return NULL;
    for (index = 0U; index < canvas->count; ++index) {
        if (strcmp(canvas->monitors[index].monitor_id, monitor_id) == 0) {
            return &canvas->monitors[index];
        }
    }
    return NULL;
}

static void monitor_canvas_refresh_bounds(UmiWorkbenchDesignerMonitorCanvas *canvas)
{
    double min_x = DBL_MAX;
    double min_y = DBL_MAX;
    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;
    size_t index;
    bool found = false;
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerMonitor *monitor = &canvas->monitors[index];
        if (!monitor->enabled) continue;
        if (monitor->bounds.x < min_x) min_x = monitor->bounds.x;
        if (monitor->bounds.y < min_y) min_y = monitor->bounds.y;
        if (monitor->bounds.x + monitor->bounds.width > max_x) max_x = monitor->bounds.x + monitor->bounds.width;
        if (monitor->bounds.y + monitor->bounds.height > max_y) max_y = monitor->bounds.y + monitor->bounds.height;
        found = true;
    }
    if (found) {
        canvas->combined_bounds.x = min_x;
        canvas->combined_bounds.y = min_y;
        canvas->combined_bounds.width = max_x - min_x;
        canvas->combined_bounds.height = max_y - min_y;
    } else {
        canvas->combined_bounds = (UmiWorkbenchDesignerRect){0.0, 0.0, 0.0, 0.0};
    }
}

UmiStatus umi_workbench_designer_monitor_canvas_add(
    UmiWorkbenchDesignerMonitorCanvas *canvas,
    const UmiWorkbenchDesignerMonitor *monitor)
{
    if (canvas == NULL || monitor == NULL || monitor->monitor_id[0] == '\0' ||
        !umi_workbench_designer_rect_is_valid(&monitor->bounds) ||
        !umi_workbench_designer_rect_is_valid(&monitor->work_area) ||
        monitor->scale <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_designer_monitor_canvas_find(canvas, monitor->monitor_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (canvas->count >= UMI_WORKBENCH_DESIGNER_MAX_MONITORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    canvas->monitors[canvas->count++] = *monitor;
    if (monitor->primary || canvas->primary_monitor_id[0] == '\0') {
        (void)umi_workbench_designer_copy_text(
            canvas->primary_monitor_id, sizeof(canvas->primary_monitor_id),
            monitor->monitor_id);
    }
    monitor_canvas_refresh_bounds(canvas);
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_monitor_canvas_remove(
    UmiWorkbenchDesignerMonitorCanvas *canvas,
    const char *monitor_id)
{
    size_t index;
    if (canvas == NULL || monitor_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < canvas->count; ++index) {
        if (strcmp(canvas->monitors[index].monitor_id, monitor_id) == 0) {
            size_t move_index;
            for (move_index = index + 1U; move_index < canvas->count; ++move_index) {
                canvas->monitors[move_index - 1U] = canvas->monitors[move_index];
            }
            canvas->count -= 1U;
            if (strcmp(canvas->primary_monitor_id, monitor_id) == 0) {
                canvas->primary_monitor_id[0] = '\0';
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

const UmiWorkbenchDesignerMonitor *umi_workbench_designer_monitor_canvas_primary(
    const UmiWorkbenchDesignerMonitorCanvas *canvas)
{
    if (canvas == NULL) return NULL;
    return umi_workbench_designer_monitor_canvas_find(
        canvas, canvas->primary_monitor_id);
}

UmiWorkbenchDesignerRect umi_workbench_designer_monitor_canvas_clamp(
    const UmiWorkbenchDesignerMonitorCanvas *canvas,
    const char *monitor_id,
    UmiWorkbenchDesignerRect bounds)
{
    const UmiWorkbenchDesignerMonitor *monitor =
        umi_workbench_designer_monitor_canvas_find(canvas, monitor_id);
    if (monitor == NULL) monitor = umi_workbench_designer_monitor_canvas_primary(canvas);
    if (monitor == NULL) return bounds;
    if (bounds.width > monitor->work_area.width) bounds.width = monitor->work_area.width;
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

UmiStatus umi_workbench_designer_monitor_canvas_relocate_missing(
    UmiWorkbenchDesignerMonitorCanvas *canvas,
    UmiWorkbenchLayoutDocument *document)
{
    const UmiWorkbenchDesignerMonitor *primary;
    size_t index;
    bool changed = false;
    if (canvas == NULL || document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    primary = umi_workbench_designer_monitor_canvas_primary(canvas);
    if (primary == NULL) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < document->node_count; ++index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[index];
        if (node->dock_region != UMI_WORKBENCH_LAYOUT_DOCK_FLOATING) continue;
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
    if (changed) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    return UMI_STATUS_OK;
}
