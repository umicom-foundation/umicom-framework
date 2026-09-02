/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/types.c
 *
 * PURPOSE:
 *   Implement textual diagnostics and geometry primitives used by every designer
 *   surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/types.h"
#include <math.h>


/*
 * Provide the workbench designer state text operation used by this module and its client
 * applications.
 */
const char *umi_workbench_designer_state_text(UmiWorkbenchDesignerState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_WORKBENCH_DESIGNER_STATE_CREATED: return "created";
        case UMI_WORKBENCH_DESIGNER_STATE_INITIALISED: return "initialised";
        case UMI_WORKBENCH_DESIGNER_STATE_RUNNING: return "running";
        case UMI_WORKBENCH_DESIGNER_STATE_QUIESCED: return "quiesced";
        case UMI_WORKBENCH_DESIGNER_STATE_STOPPED: return "stopped";
        case UMI_WORKBENCH_DESIGNER_STATE_FAILED: return "failed";
        default: return "unknown";
    }
}

/*
 * Provide the workbench designer mode text operation used by this module and its client
 * applications.
 */
const char *umi_workbench_designer_mode_text(UmiWorkbenchDesignerMode mode)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (mode) {
        case UMI_WORKBENCH_DESIGNER_MODE_BROWSE: return "browse";
        case UMI_WORKBENCH_DESIGNER_MODE_DESIGN: return "design";
        case UMI_WORKBENCH_DESIGNER_MODE_PREVIEW: return "preview";
        case UMI_WORKBENCH_DESIGNER_MODE_REVIEW: return "review";
        case UMI_WORKBENCH_DESIGNER_MODE_CONFLICT: return "conflict";
        default: return "unknown";
    }
}

/*
 * Provide the workbench designer tool text operation used by this module and its client
 * applications.
 */
const char *umi_workbench_designer_tool_text(UmiWorkbenchDesignerTool tool)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (tool) {
        case UMI_WORKBENCH_DESIGNER_TOOL_SELECT: return "select";
        case UMI_WORKBENCH_DESIGNER_TOOL_PAN: return "pan";
        case UMI_WORKBENCH_DESIGNER_TOOL_SPLIT_HORIZONTAL: return "split-horizontal";
        case UMI_WORKBENCH_DESIGNER_TOOL_SPLIT_VERTICAL: return "split-vertical";
        case UMI_WORKBENCH_DESIGNER_TOOL_TAB_GROUP: return "tab-group";
        case UMI_WORKBENCH_DESIGNER_TOOL_FLOAT: return "float";
        case UMI_WORKBENCH_DESIGNER_TOOL_DOCK: return "dock";
        case UMI_WORKBENCH_DESIGNER_TOOL_LINK_CONTEXT: return "link-context";
        default: return "unknown";
    }
}

/*
 * Provide the workbench designer drop zone text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_designer_drop_zone_text(UmiWorkbenchDesignerDropZone zone)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (zone) {
        case UMI_WORKBENCH_DESIGNER_DROP_NONE: return "none";
        case UMI_WORKBENCH_DESIGNER_DROP_LEFT: return "left";
        case UMI_WORKBENCH_DESIGNER_DROP_RIGHT: return "right";
        case UMI_WORKBENCH_DESIGNER_DROP_TOP: return "top";
        case UMI_WORKBENCH_DESIGNER_DROP_BOTTOM: return "bottom";
        case UMI_WORKBENCH_DESIGNER_DROP_CENTRE: return "centre";
        case UMI_WORKBENCH_DESIGNER_DROP_TAB: return "tab";
        case UMI_WORKBENCH_DESIGNER_DROP_FLOATING: return "floating";
        default: return "unknown";
    }
}

/*
 * Provide the workbench designer command kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_designer_command_kind_text(UmiWorkbenchDesignerCommandKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_WORKBENCH_DESIGNER_COMMAND_NONE: return "none";
        case UMI_WORKBENCH_DESIGNER_COMMAND_ADD_PANEL: return "add-panel";
        case UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE: return "remove-node";
        case UMI_WORKBENCH_DESIGNER_COMMAND_MOVE_NODE: return "move-node";
        case UMI_WORKBENCH_DESIGNER_COMMAND_SET_PROPERTY: return "set-property";
        case UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL: return "split-horizontal";
        case UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_VERTICAL: return "split-vertical";
        case UMI_WORKBENCH_DESIGNER_COMMAND_CREATE_TAB_GROUP: return "create-tab-group";
        case UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE: return "float-node";
        case UMI_WORKBENCH_DESIGNER_COMMAND_DOCK_NODE: return "dock-node";
        case UMI_WORKBENCH_DESIGNER_COMMAND_REORDER_TAB: return "reorder-tab";
        case UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB: return "set-active-tab";
        case UMI_WORKBENCH_DESIGNER_COMMAND_RENAME_LAYOUT: return "rename-layout";
        case UMI_WORKBENCH_DESIGNER_COMMAND_LOCK_LAYOUT: return "lock-layout";
        case UMI_WORKBENCH_DESIGNER_COMMAND_UNLOCK_LAYOUT: return "unlock-layout";
        case UMI_WORKBENCH_DESIGNER_COMMAND_UNDO: return "undo";
        case UMI_WORKBENCH_DESIGNER_COMMAND_REDO: return "redo";
        case UMI_WORKBENCH_DESIGNER_COMMAND_SAVE: return "save";
        case UMI_WORKBENCH_DESIGNER_COMMAND_IMPORT: return "import";
        case UMI_WORKBENCH_DESIGNER_COMMAND_EXPORT: return "export";
        default: return "unknown";
    }
}

/*
 * Provide the workbench designer save state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_designer_save_state_text(UmiWorkbenchDesignerSaveState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_WORKBENCH_DESIGNER_SAVE_CLEAN: return "clean";
        case UMI_WORKBENCH_DESIGNER_SAVE_DIRTY: return "dirty";
        case UMI_WORKBENCH_DESIGNER_SAVE_PENDING: return "pending";
        case UMI_WORKBENCH_DESIGNER_SAVE_SAVING: return "saving";
        case UMI_WORKBENCH_DESIGNER_SAVE_FAILED: return "failed";
        case UMI_WORKBENCH_DESIGNER_SAVE_CONFLICT: return "conflict";
        default: return "unknown";
    }
}

/*
 * Check that workbench designer rect satisfies its contract before another service relies
 * on it.
 */
bool umi_workbench_designer_rect_is_valid(const UmiWorkbenchDesignerRect *rect)
{
    return rect != NULL && isfinite(rect->x) && isfinite(rect->y) &&
           isfinite(rect->width) && isfinite(rect->height) &&
           rect->width >= 0.0 && rect->height >= 0.0;
}

/*
 * Provide the workbench designer rect contains point operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_rect_contains_point(
    const UmiWorkbenchDesignerRect *rect,
    UmiWorkbenchDesignerPoint point)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_rect_is_valid(rect)) return false;
    return point.x >= rect->x && point.y >= rect->y &&
           point.x <= rect->x + rect->width &&
           point.y <= rect->y + rect->height;
}

/*
 * Provide the workbench designer rect intersects operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_rect_intersects(
    const UmiWorkbenchDesignerRect *left,
    const UmiWorkbenchDesignerRect *right)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_rect_is_valid(left) ||
        !umi_workbench_designer_rect_is_valid(right)) return false;
    return left->x < right->x + right->width &&
           left->x + left->width > right->x &&
           left->y < right->y + right->height &&
           left->y + left->height > right->y;
}

/*
 * Provide the workbench designer rect intersection operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerRect umi_workbench_designer_rect_intersection(
    const UmiWorkbenchDesignerRect *left,
    const UmiWorkbenchDesignerRect *right)
{
    UmiWorkbenchDesignerRect result = {0.0, 0.0, 0.0, 0.0};
    double right_edge;
    double bottom_edge;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_workbench_designer_rect_intersects(left, right)) return result;
    result.x = left->x > right->x ? left->x : right->x;
    result.y = left->y > right->y ? left->y : right->y;
    right_edge = (left->x + left->width) < (right->x + right->width)
        ? left->x + left->width : right->x + right->width;
    bottom_edge = (left->y + left->height) < (right->y + right->height)
        ? left->y + left->height : right->y + right->height;
    result.width = right_edge - result.x;
    result.height = bottom_edge - result.y;
    return result;
}

/*
 * Provide the workbench designer clamp operation used by this module and its client
 * applications.
 */
double umi_workbench_designer_clamp(double value, double minimum, double maximum)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value < minimum) return minimum;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value > maximum) return maximum;
    return value;
}
