/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/types.c
 *
 * PURPOSE:
 *   Implement stable text conversion and geometry helpers for the workbench layout public value types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/types.h"

#include <limits.h>

const char *umi_workbench_layout_node_kind_text(
    UmiWorkbenchLayoutNodeKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_NODE_EMPTY: return "empty";
    case UMI_WORKBENCH_LAYOUT_NODE_WINDOW: return "window";
    case UMI_WORKBENCH_LAYOUT_NODE_SPLIT: return "split";
    case UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP: return "tab-group";
    case UMI_WORKBENCH_LAYOUT_NODE_PANEL: return "panel";
    case UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP: return "editor-group";
    case UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW:
        return "floating-window";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_orientation_text(
    UmiWorkbenchLayoutOrientation orientation)
{
    switch (orientation) {
    case UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE: return "none";
    case UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL:
        return "horizontal";
    case UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL:
        return "vertical";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_dock_region_text(
    UmiWorkbenchLayoutDockRegion region)
{
    switch (region) {
    case UMI_WORKBENCH_LAYOUT_DOCK_CANVAS: return "canvas";
    case UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT: return "document";
    case UMI_WORKBENCH_LAYOUT_DOCK_LEFT: return "left";
    case UMI_WORKBENCH_LAYOUT_DOCK_RIGHT: return "right";
    case UMI_WORKBENCH_LAYOUT_DOCK_TOP: return "top";
    case UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM: return "bottom";
    case UMI_WORKBENCH_LAYOUT_DOCK_FLOATING: return "floating";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_visibility_text(
    UmiWorkbenchLayoutVisibility visibility)
{
    switch (visibility) {
    case UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE: return "visible";
    case UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN: return "hidden";
    case UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO: return "auto";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_severity_text(
    UmiWorkbenchLayoutSeverity severity)
{
    switch (severity) {
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INFO: return "info";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING: return "warning";
    case UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR: return "error";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_operation_kind_text(
    UmiWorkbenchLayoutOperationKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE: return "add-node";
    case UMI_WORKBENCH_LAYOUT_OPERATION_REMOVE_NODE: return "remove-node";
    case UMI_WORKBENCH_LAYOUT_OPERATION_MOVE_NODE: return "move-node";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ROOT: return "set-root";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO:
        return "set-split-ratio";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ACTIVE_CHILD:
        return "set-active-child";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS: return "set-bounds";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY:
        return "set-visibility";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP:
        return "set-context-group";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT:
        return "set-component";
    case UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT:
        return "rename-layout";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED: return "set-locked";
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY: return "set-dirty";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_diff_kind_text(
    UmiWorkbenchLayoutDiffKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED:
        return "metadata-changed";
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_ADDED: return "node-added";
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_REMOVED: return "node-removed";
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_MOVED: return "node-moved";
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED: return "node-changed";
    case UMI_WORKBENCH_LAYOUT_DIFF_ORDER_CHANGED: return "order-changed";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_conflict_kind_text(
    UmiWorkbenchLayoutConflictKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_CONFLICT_METADATA:
        return "metadata";
    case UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_ADDED_DIFFERENTLY:
        return "node-added-differently";
    case UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_REMOVED_AND_CHANGED:
        return "node-removed-and-changed";
    case UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_CHANGED_DIFFERENTLY:
        return "node-changed-differently";
    case UMI_WORKBENCH_LAYOUT_CONFLICT_PARENT_CHANGED_DIFFERENTLY:
        return "parent-changed-differently";
    case UMI_WORKBENCH_LAYOUT_CONFLICT_ORDER_CHANGED_DIFFERENTLY:
        return "order-changed-differently";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_store_kind_text(
    UmiWorkbenchLayoutStoreKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_STORE_MEMORY: return "memory";
    case UMI_WORKBENCH_LAYOUT_STORE_DATA_SERVER: return "data-server";
    case UMI_WORKBENCH_LAYOUT_STORE_FILE: return "file";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_event_kind_text(
    UmiWorkbenchLayoutEventKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_EVENT_CREATED: return "created";
    case UMI_WORKBENCH_LAYOUT_EVENT_UPDATED: return "updated";
    case UMI_WORKBENCH_LAYOUT_EVENT_DELETED: return "deleted";
    case UMI_WORKBENCH_LAYOUT_EVENT_ACTIVATED: return "activated";
    case UMI_WORKBENCH_LAYOUT_EVENT_SAVED: return "saved";
    case UMI_WORKBENCH_LAYOUT_EVENT_RESTORED: return "restored";
    case UMI_WORKBENCH_LAYOUT_EVENT_IMPORTED: return "imported";
    case UMI_WORKBENCH_LAYOUT_EVENT_EXPORTED: return "exported";
    case UMI_WORKBENCH_LAYOUT_EVENT_RECOVERY_REQUIRED:
        return "recovery-required";
    case UMI_WORKBENCH_LAYOUT_EVENT_CONFLICT_DETECTED:
        return "conflict-detected";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_command_kind_text(
    UmiWorkbenchLayoutCommandKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_COMMAND_CREATE: return "create";
    case UMI_WORKBENCH_LAYOUT_COMMAND_CLONE: return "clone";
    case UMI_WORKBENCH_LAYOUT_COMMAND_DELETE: return "delete";
    case UMI_WORKBENCH_LAYOUT_COMMAND_ACTIVATE: return "activate";
    case UMI_WORKBENCH_LAYOUT_COMMAND_APPLY_OPERATION:
        return "apply-operation";
    case UMI_WORKBENCH_LAYOUT_COMMAND_UNDO: return "undo";
    case UMI_WORKBENCH_LAYOUT_COMMAND_REDO: return "redo";
    case UMI_WORKBENCH_LAYOUT_COMMAND_SAVE: return "save";
    case UMI_WORKBENCH_LAYOUT_COMMAND_RESTORE: return "restore";
    case UMI_WORKBENCH_LAYOUT_COMMAND_IMPORT: return "import";
    case UMI_WORKBENCH_LAYOUT_COMMAND_EXPORT: return "export";
    case UMI_WORKBENCH_LAYOUT_COMMAND_LOCK: return "lock";
    case UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK: return "unlock";
    default: return "invalid";
    }
}

const char *umi_workbench_layout_controller_state_text(
    UmiWorkbenchLayoutControllerState state)
{
    switch (state) {
    case UMI_WORKBENCH_LAYOUT_CONTROLLER_CREATED: return "created";
    case UMI_WORKBENCH_LAYOUT_CONTROLLER_INITIALISED: return "initialised";
    case UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING: return "running";
    case UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED: return "quiesced";
    case UMI_WORKBENCH_LAYOUT_CONTROLLER_STOPPED: return "stopped";
    case UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED: return "failed";
    default: return "invalid";
    }
}

bool umi_workbench_layout_rect_is_valid(
    const UmiWorkbenchLayoutRect *rect)
{
    return rect != NULL && rect->width >= 0 && rect->height >= 0;
}

bool umi_workbench_layout_rect_contains(
    const UmiWorkbenchLayoutRect *outer,
    const UmiWorkbenchLayoutRect *inner)
{
    int64_t outer_right;
    int64_t outer_bottom;
    int64_t inner_right;
    int64_t inner_bottom;

    if (!umi_workbench_layout_rect_is_valid(outer) ||
        !umi_workbench_layout_rect_is_valid(inner)) {
        return false;
    }

    outer_right = (int64_t)outer->x + outer->width;
    outer_bottom = (int64_t)outer->y + outer->height;
    inner_right = (int64_t)inner->x + inner->width;
    inner_bottom = (int64_t)inner->y + inner->height;

    return inner->x >= outer->x && inner->y >= outer->y &&
           inner_right <= outer_right && inner_bottom <= outer_bottom;
}

bool umi_workbench_layout_rect_intersects(
    const UmiWorkbenchLayoutRect *left,
    const UmiWorkbenchLayoutRect *right)
{
    int64_t left_right;
    int64_t left_bottom;
    int64_t right_right;
    int64_t right_bottom;

    if (!umi_workbench_layout_rect_is_valid(left) ||
        !umi_workbench_layout_rect_is_valid(right)) {
        return false;
    }

    left_right = (int64_t)left->x + left->width;
    left_bottom = (int64_t)left->y + left->height;
    right_right = (int64_t)right->x + right->width;
    right_bottom = (int64_t)right->y + right->height;

    return left->x < right_right && left_right > right->x &&
           left->y < right_bottom && left_bottom > right->y;
}

UmiWorkbenchLayoutRect umi_workbench_layout_rect_intersection(
    const UmiWorkbenchLayoutRect *left,
    const UmiWorkbenchLayoutRect *right)
{
    UmiWorkbenchLayoutRect intersection = {0, 0, 0, 0};
    int64_t x1;
    int64_t y1;
    int64_t x2;
    int64_t y2;

    if (!umi_workbench_layout_rect_intersects(left, right)) {
        return intersection;
    }

    x1 = left->x > right->x ? left->x : right->x;
    y1 = left->y > right->y ? left->y : right->y;
    x2 = ((int64_t)left->x + left->width) <
                 ((int64_t)right->x + right->width)
             ? ((int64_t)left->x + left->width)
             : ((int64_t)right->x + right->width);
    y2 = ((int64_t)left->y + left->height) <
                 ((int64_t)right->y + right->height)
             ? ((int64_t)left->y + left->height)
             : ((int64_t)right->y + right->height);

    if (x1 < INT32_MIN || x1 > INT32_MAX ||
        y1 < INT32_MIN || y1 > INT32_MAX ||
        x2 - x1 > INT32_MAX || y2 - y1 > INT32_MAX) {
        return intersection;
    }

    intersection.x = (int32_t)x1;
    intersection.y = (int32_t)y1;
    intersection.width = (int32_t)(x2 - x1);
    intersection.height = (int32_t)(y2 - y1);
    return intersection;
}

int64_t umi_workbench_layout_rect_area(
    const UmiWorkbenchLayoutRect *rect)
{
    if (!umi_workbench_layout_rect_is_valid(rect)) {
        return 0;
    }
    return (int64_t)rect->width * (int64_t)rect->height;
}
