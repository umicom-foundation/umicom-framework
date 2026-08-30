/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/drag.c
 *
 * PURPOSE:
 *   Implement drag lifecycle without coupling to GTK gesture objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/drag.h"
#include "internal.h"


void umi_workbench_designer_drag_init(UmiWorkbenchDesignerDragSession *drag)
{
    if (drag == NULL) return;
    (void)memset(drag, 0, sizeof(*drag));
    drag->phase = UMI_WORKBENCH_DESIGNER_DRAG_IDLE;
    drag->proposed_zone = UMI_WORKBENCH_DESIGNER_DROP_NONE;
}

UmiStatus umi_workbench_designer_drag_begin(
    UmiWorkbenchDesignerDragSession *drag,
    const char *node_id,
    UmiWorkbenchDesignerPoint pointer,
    UmiWorkbenchDesignerRect bounds,
    uint64_t timestamp_ms)
{
    if (drag == NULL || node_id == NULL || node_id[0] == '\0' ||
        !umi_workbench_designer_rect_is_valid(&bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_drag_init(drag);
    if (umi_workbench_designer_copy_text(
            drag->node_id, sizeof(drag->node_id), node_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    drag->phase = UMI_WORKBENCH_DESIGNER_DRAG_ARMED;
    drag->start_point = pointer;
    drag->current_point = pointer;
    drag->pointer_offset.x = pointer.x - bounds.x;
    drag->pointer_offset.y = pointer.y - bounds.y;
    drag->original_bounds = bounds;
    drag->preview_bounds = bounds;
    drag->started_at_ms = timestamp_ms;
    drag->updated_at_ms = timestamp_ms;
    drag->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_drag_update(
    UmiWorkbenchDesignerDragSession *drag,
    UmiWorkbenchDesignerPoint pointer,
    uint64_t timestamp_ms)
{
    if (drag == NULL || (drag->phase != UMI_WORKBENCH_DESIGNER_DRAG_ARMED &&
        drag->phase != UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE)) {
        return UMI_STATUS_INVALID_STATE;
    }
    drag->phase = UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE;
    drag->current_point = pointer;
    drag->preview_bounds.x = pointer.x - drag->pointer_offset.x;
    drag->preview_bounds.y = pointer.y - drag->pointer_offset.y;
    drag->updated_at_ms = timestamp_ms;
    drag->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_drag_set_target(
    UmiWorkbenchDesignerDragSession *drag,
    const char *target_node_id,
    UmiWorkbenchDesignerDropZone zone)
{
    if (drag == NULL || drag->phase != UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE) {
        return UMI_STATUS_INVALID_STATE;
    }
    drag->proposed_target_node_id[0] = '\0';
    if (target_node_id != NULL) {
        UmiStatus status = umi_workbench_designer_copy_text(
            drag->proposed_target_node_id,
            sizeof(drag->proposed_target_node_id),
            target_node_id);
        if (status != UMI_STATUS_OK) return status;
    }
    drag->proposed_zone = zone;
    drag->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_drag_complete(
    UmiWorkbenchDesignerDragSession *drag)
{
    if (drag == NULL || drag->phase != UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE ||
        drag->proposed_zone == UMI_WORKBENCH_DESIGNER_DROP_NONE) {
        return UMI_STATUS_INVALID_STATE;
    }
    drag->phase = UMI_WORKBENCH_DESIGNER_DRAG_COMPLETED;
    drag->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_workbench_designer_drag_cancel(UmiWorkbenchDesignerDragSession *drag)
{
    if (drag == NULL) return;
    drag->phase = UMI_WORKBENCH_DESIGNER_DRAG_CANCELLED;
    drag->preview_bounds = drag->original_bounds;
    drag->proposed_zone = UMI_WORKBENCH_DESIGNER_DROP_NONE;
    drag->proposed_target_node_id[0] = '\0';
    drag->revision += 1U;
}
