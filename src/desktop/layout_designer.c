/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/layout_designer.c
 *
 * PURPOSE:
 *   Implement protected-layout cloning, snapping, dock placement and bounded
 *   transactional history for every Umicom application's visual workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/layout_designer.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesktopLayoutDesigner {
    UmiDesktopRuntime *runtime;
    UmiDesktopSnapPolicy policy;
    UmiDesktopLayout baseline;
    UmiDesktopLayout undo[UMI_DESKTOP_DESIGNER_HISTORY_DEPTH];
    UmiDesktopLayout redo[UMI_DESKTOP_DESIGNER_HISTORY_DEPTH];
    size_t undo_count;
    size_t redo_count;
    char source_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char working_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char selected_window_id[UMI_DESKTOP_ID_CAPACITY];
    uint32_t snap_guides;
    bool active;
    bool created_copy;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the working layout operation used by this module and its client applications. */
static const UmiDesktopLayout *working_layout(
    const UmiDesktopLayoutDesigner *designer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || designer->runtime == NULL ||
        designer->working_layout_id[0] == '\0') return NULL;
    return umi_desktop_layout_catalogue_find(
        umi_desktop_runtime_layouts(designer->runtime),
        designer->working_layout_id);
}

/* Provide the selected window operation used by this module and its client applications. */
static const UmiDesktopWindow *selected_window(
    const UmiDesktopLayoutDesigner *designer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || designer->selected_window_id[0] == '\0')
        return NULL;
    return umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(designer->runtime),
        designer->selected_window_id);
}

/* Provide the push layout operation used by this module and its client applications. */
static void push_layout(
    UmiDesktopLayout *history,
    size_t *count,
    const UmiDesktopLayout *layout)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count == UMI_DESKTOP_DESIGNER_HISTORY_DEPTH) {
        (void)memmove(&history[0], &history[1],
                      (UMI_DESKTOP_DESIGNER_HISTORY_DEPTH - 1U) *
                          sizeof(history[0]));
        *count -= 1U;
    }
    history[*count] = *layout;
    *count += 1U;
}

/* Provide the checkpoint operation used by this module and its client applications. */
static UmiStatus checkpoint(UmiDesktopLayoutDesigner *designer)
{
    const UmiDesktopLayout *layout = working_layout(designer);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    push_layout(designer->undo, &designer->undo_count, layout);
    designer->redo_count = 0U;
    return UMI_STATUS_OK;
}

/*
 * Provide the rollback checkpoint operation used by this module and its client
 * applications.
 */
static void rollback_checkpoint(UmiDesktopLayoutDesigner *designer)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (designer->undo_count > 0U) designer->undo_count -= 1U;
}

/*
 * Initialise desktop layout designer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_layout_designer_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopLayoutDesigner **out_designer)
{
    UmiDesktopLayoutDesigner *designer;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_designer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_designer = NULL;
    designer = (UmiDesktopLayoutDesigner *)calloc(1U, sizeof(*designer));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    designer->runtime = runtime;
    umi_desktop_snap_policy_init(&designer->policy);
    designer->revision = 1U;
    *out_designer = designer;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by desktop layout designer so the same storage can be reused
 * safely.
 */
void umi_desktop_layout_designer_destroy(UmiDesktopLayoutDesigner *designer)
{
    free(designer);
}

/*
 * Provide the desktop layout designer begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_begin(
    UmiDesktopLayoutDesigner *designer,
    const char *source_layout_id,
    const char *working_layout_id,
    const char *working_name)
{
    const UmiDesktopLayout *source;
    const UmiDesktopLayout *working;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || source_layout_id == NULL ||
        working_layout_id == NULL || working_name == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (designer->active) return UMI_STATUS_INVALID_STATE;
    source = umi_desktop_layout_catalogue_find(
        umi_desktop_runtime_layouts(designer->runtime), source_layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_text(designer->source_layout_id,
                       sizeof(designer->source_layout_id), source_layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(designer->working_layout_id,
                           sizeof(designer->working_layout_id),
                           working_layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    designer->baseline = *source;
    designer->created_copy = source->locked || source->built_in ||
        strcmp(source_layout_id, working_layout_id) != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (designer->created_copy) {
        status = umi_desktop_runtime_clone_layout(
            designer->runtime, source_layout_id, working_layout_id,
            working_name, true);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_desktop_runtime_activate_layout(
            designer->runtime, source_layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(designer->source_layout_id, 0,
                     sizeof(designer->source_layout_id));
        (void)memset(designer->working_layout_id, 0,
                     sizeof(designer->working_layout_id));
        return status;
    }
    working = working_layout(designer);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (working == NULL || working->locked || working->built_in)
        return UMI_STATUS_INVALID_STATE;
    designer->undo_count = 0U;
    designer->redo_count = 0U;
    designer->snap_guides = 0U;
    designer->active = true;
    /* Apply this branch only when its contract condition is satisfied. */
    if (working->window_count > 0U) {
        (void)copy_text(designer->selected_window_id,
                        sizeof(designer->selected_window_id),
                        working->windows[0].window_id);
    }
    designer->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout designer select operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_select(
    UmiDesktopLayoutDesigner *designer,
    const char *window_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!designer->active) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_window_manager_find(
            umi_desktop_runtime_windows(designer->runtime), window_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (copy_text(designer->selected_window_id,
                  sizeof(designer->selected_window_id), window_id) !=
        UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    designer->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find desktop layout designer place while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_layout_designer_place_selected(
    UmiDesktopLayoutDesigner *designer,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement)
{
    const UmiDesktopMonitor *monitor;
    UmiDesktopSnapResult result;
    char target_monitor_id[UMI_DESKTOP_ID_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || monitor_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!designer->active || selected_window(designer) == NULL)
        return UMI_STATUS_INVALID_STATE;
    status = copy_text(target_monitor_id, sizeof(target_monitor_id), monitor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    monitor = umi_desktop_monitor_topology_find(
        umi_desktop_runtime_monitors(designer->runtime), target_monitor_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_desktop_snap_engine_apply(
        &designer->policy, monitor,
        umi_desktop_runtime_windows(designer->runtime),
        designer->selected_window_id, bounds, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = checkpoint(designer);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_desktop_runtime_place_window(
        designer->runtime, designer->selected_window_id, target_monitor_id,
        result.bounds, placement);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        rollback_checkpoint(designer);
        return status;
    }
    designer->snap_guides = result.guides;
    designer->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find desktop layout designer move while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_layout_designer_move_selected(
    UmiDesktopLayoutDesigner *designer,
    int32_t delta_x,
    int32_t delta_y)
{
    const UmiDesktopWindow *window = selected_window(designer);
    UmiDesktopRect bounds;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    bounds = window->bounds;
    bounds.x += delta_x;
    bounds.y += delta_y;
    return umi_desktop_layout_designer_place_selected(
        designer, window->monitor_id, bounds, UMI_DESKTOP_DOCK_FLOATING);
}

/*
 * Find desktop layout designer resize while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_desktop_layout_designer_resize_selected(
    UmiDesktopLayoutDesigner *designer,
    UmiDesktopResizeEdge edge,
    int32_t delta_x,
    int32_t delta_y)
{
    const UmiDesktopWindow *window = selected_window(designer);
    UmiDesktopRect bounds;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (edge < UMI_DESKTOP_RESIZE_LEFT ||
        edge > UMI_DESKTOP_RESIZE_BOTTOM_RIGHT)
        return UMI_STATUS_INVALID_ARGUMENT;
    bounds = window->bounds;
    /* Apply this branch only when its contract condition is satisfied. */
    if (edge == UMI_DESKTOP_RESIZE_LEFT ||
        edge == UMI_DESKTOP_RESIZE_TOP_LEFT ||
        edge == UMI_DESKTOP_RESIZE_BOTTOM_LEFT) {
        bounds.x += delta_x;
        bounds.width -= delta_x;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (edge == UMI_DESKTOP_RESIZE_RIGHT ||
        edge == UMI_DESKTOP_RESIZE_TOP_RIGHT ||
        edge == UMI_DESKTOP_RESIZE_BOTTOM_RIGHT)
        bounds.width += delta_x;
    /* Apply this branch only when its contract condition is satisfied. */
    if (edge == UMI_DESKTOP_RESIZE_TOP ||
        edge == UMI_DESKTOP_RESIZE_TOP_LEFT ||
        edge == UMI_DESKTOP_RESIZE_TOP_RIGHT) {
        bounds.y += delta_y;
        bounds.height -= delta_y;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (edge == UMI_DESKTOP_RESIZE_BOTTOM ||
        edge == UMI_DESKTOP_RESIZE_BOTTOM_LEFT ||
        edge == UMI_DESKTOP_RESIZE_BOTTOM_RIGHT)
        bounds.height += delta_y;
    return umi_desktop_layout_designer_place_selected(
        designer, window->monitor_id, bounds, UMI_DESKTOP_DOCK_FLOATING);
}

/*
 * Find desktop layout designer dock while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_layout_designer_dock_selected(
    UmiDesktopLayoutDesigner *designer,
    UmiDesktopDockPlacement placement)
{
    const UmiDesktopWindow *window = selected_window(designer);
    const UmiDesktopMonitor *monitor;
    UmiDesktopRect bounds;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    monitor = umi_desktop_monitor_topology_find(
        umi_desktop_runtime_monitors(designer->runtime), window->monitor_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) return UMI_STATUS_NOT_FOUND;
    bounds = monitor->work_area;
    /* Select the behaviour associated with the requested command or state value. */
    switch (placement) {
        case UMI_DESKTOP_DOCK_LEFT:
            bounds.width /= 2;
            break;
        case UMI_DESKTOP_DOCK_RIGHT:
            bounds.width /= 2;
            bounds.x += monitor->work_area.width - bounds.width;
            break;
        case UMI_DESKTOP_DOCK_TOP:
            bounds.height /= 2;
            break;
        case UMI_DESKTOP_DOCK_BOTTOM:
            bounds.height /= 2;
            bounds.y += monitor->work_area.height - bounds.height;
            break;
        case UMI_DESKTOP_DOCK_DOCUMENT:
        case UMI_DESKTOP_DOCK_CANVAS:
            break;
        case UMI_DESKTOP_DOCK_FLOATING:
            bounds.width = monitor->work_area.width * 7 / 10;
            bounds.height = monitor->work_area.height * 7 / 10;
            bounds.x = monitor->work_area.x +
                (monitor->work_area.width - bounds.width) / 2;
            bounds.y = monitor->work_area.y +
                (monitor->work_area.height - bounds.height) / 2;
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_desktop_layout_designer_place_selected(
        designer, monitor->monitor_id, bounds, placement);
}

/*
 * Provide the desktop layout designer undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_undo(
    UmiDesktopLayoutDesigner *designer)
{
    const UmiDesktopLayout *current;
    UmiDesktopLayout previous;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (!designer->active || designer->undo_count == 0U)
        return UMI_STATUS_INVALID_STATE;
    current = working_layout(designer);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (current == NULL) return UMI_STATUS_INVALID_STATE;
    previous = designer->undo[designer->undo_count - 1U];
    push_layout(designer->redo, &designer->redo_count, current);
    designer->undo_count -= 1U;
    status = umi_desktop_runtime_replace_layout(
        designer->runtime, &previous, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) designer->revision += 1U;
    return status;
}

/*
 * Provide the desktop layout designer redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_redo(
    UmiDesktopLayoutDesigner *designer)
{
    const UmiDesktopLayout *current;
    UmiDesktopLayout next;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (!designer->active || designer->redo_count == 0U)
        return UMI_STATUS_INVALID_STATE;
    current = working_layout(designer);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (current == NULL) return UMI_STATUS_INVALID_STATE;
    next = designer->redo[designer->redo_count - 1U];
    push_layout(designer->undo, &designer->undo_count, current);
    designer->redo_count -= 1U;
    status = umi_desktop_runtime_replace_layout(designer->runtime, &next, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) designer->revision += 1U;
    return status;
}

/*
 * Provide the desktop layout designer commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_commit(
    UmiDesktopLayoutDesigner *designer)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!designer->active) return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_runtime_commit_layout(
        designer->runtime, designer->working_layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        designer->active = false;
        designer->undo_count = 0U;
        designer->redo_count = 0U;
        designer->snap_guides = 0U;
        designer->revision += 1U;
    }
    return status;
}

/*
 * Provide the desktop layout designer cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_designer_cancel(
    UmiDesktopLayoutDesigner *designer)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!designer->active) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (designer->created_copy) {
        status = umi_desktop_runtime_activate_layout(
            designer->runtime, designer->source_layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = umi_desktop_runtime_remove_layout(
                designer->runtime, designer->working_layout_id);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_desktop_runtime_replace_layout(
            designer->runtime, &designer->baseline, true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = umi_desktop_runtime_commit_layout(
                designer->runtime, designer->working_layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        designer->active = false;
        designer->undo_count = 0U;
        designer->redo_count = 0U;
        designer->snap_guides = 0U;
        designer->revision += 1U;
    }
    return status;
}

/*
 * Provide the desktop layout designer set snap policy operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_layout_designer_set_snap_policy(
    UmiDesktopLayoutDesigner *designer,
    const UmiDesktopSnapPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || policy == NULL || policy->grid_size <= 0 ||
        policy->snap_distance < 0 || policy->minimum_width <= 0 ||
        policy->minimum_height <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    designer->policy = *policy;
    designer->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout designer snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_layout_designer_snapshot(
    const UmiDesktopLayoutDesigner *designer,
    UmiDesktopLayoutDesignerSnapshot *out_snapshot)
{
    const UmiDesktopLayout *layout;
    const UmiDesktopLayoutTab *tab;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (designer->source_layout_id[0] != '\0') {
        status = copy_text(out_snapshot->source_layout_id,
                           sizeof(out_snapshot->source_layout_id),
                           designer->source_layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (designer->working_layout_id[0] != '\0') {
        status = copy_text(out_snapshot->working_layout_id,
                           sizeof(out_snapshot->working_layout_id),
                           designer->working_layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (designer->selected_window_id[0] != '\0') {
        status = copy_text(out_snapshot->selected_window_id,
                           sizeof(out_snapshot->selected_window_id),
                           designer->selected_window_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    layout = working_layout(designer);
    tab = designer->working_layout_id[0] != '\0'
        ? umi_desktop_layout_tabs_find(
              umi_desktop_runtime_tabs(designer->runtime),
              designer->working_layout_id)
        : NULL;
    out_snapshot->window_count = layout != NULL ? layout->window_count : 0U;
    out_snapshot->undo_count = designer->undo_count;
    out_snapshot->redo_count = designer->redo_count;
    out_snapshot->snap_guides = designer->snap_guides;
    out_snapshot->active = designer->active;
    out_snapshot->created_copy = designer->created_copy;
    out_snapshot->dirty = tab != NULL && tab->dirty;
    out_snapshot->can_commit = designer->active && layout != NULL &&
        !layout->locked && !layout->built_in;
    out_snapshot->revision = designer->revision;
    return UMI_STATUS_OK;
}
