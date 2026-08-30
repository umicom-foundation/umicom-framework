/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/drag_operation.c
 *
 * PURPOSE:
 *   Select the structural command implied by a docking zone and preserve preview
 *   and audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/drag_operation.h"
#include "internal.h"


UmiStatus umi_workbench_designer_drag_operation_plan(
    const UmiWorkbenchDesignerDragSession *drag,
    const UmiWorkbenchDesignerDropTarget *target,
    const char *command_id,
    const char *actor_id,
    uint64_t expected_revision,
    UmiWorkbenchDesignerDragOperationPlan *out_plan)
{
    UmiWorkbenchDesignerCommandKind kind;
    if (drag == NULL || target == NULL || command_id == NULL ||
        actor_id == NULL || out_plan == NULL ||
        drag->phase != UMI_WORKBENCH_DESIGNER_DRAG_ACTIVE || !target->enabled ||
        target->zone == UMI_WORKBENCH_DESIGNER_DROP_NONE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    switch (target->zone) {
        case UMI_WORKBENCH_DESIGNER_DROP_LEFT:
        case UMI_WORKBENCH_DESIGNER_DROP_RIGHT:
            kind = UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL;
            out_plan->creates_container = true;
            break;
        case UMI_WORKBENCH_DESIGNER_DROP_TOP:
        case UMI_WORKBENCH_DESIGNER_DROP_BOTTOM:
            kind = UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_VERTICAL;
            out_plan->creates_container = true;
            break;
        case UMI_WORKBENCH_DESIGNER_DROP_CENTRE:
        case UMI_WORKBENCH_DESIGNER_DROP_TAB:
            kind = UMI_WORKBENCH_DESIGNER_COMMAND_CREATE_TAB_GROUP;
            out_plan->creates_tab_group = true;
            break;
        case UMI_WORKBENCH_DESIGNER_DROP_FLOATING:
            kind = UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE;
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_command_init(&out_plan->command, kind, command_id);
    (void)umi_workbench_designer_copy_text(
        out_plan->command.actor_id, sizeof(out_plan->command.actor_id), actor_id);
    (void)umi_workbench_designer_copy_text(
        out_plan->command.target_node_id,
        sizeof(out_plan->command.target_node_id), drag->node_id);
    (void)umi_workbench_designer_copy_text(
        out_plan->command.parent_node_id,
        sizeof(out_plan->command.parent_node_id), target->target_node_id);
    out_plan->command.drop_zone = target->zone;
    out_plan->command.rect_value = target->preview_bounds;
    out_plan->command.expected_revision = expected_revision;
    out_plan->preview_bounds = target->preview_bounds;
    out_plan->valid = true;
    (void)umi_workbench_designer_copy_text(
        out_plan->explanation, sizeof(out_plan->explanation),
        out_plan->creates_tab_group
            ? "Drop creates or extends a semantic tab group."
            : out_plan->creates_container
                ? "Drop creates a semantic split container."
                : "Drop creates a floating semantic window.");
    return UMI_STATUS_OK;
}
