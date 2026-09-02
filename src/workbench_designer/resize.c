/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/resize.c
 *
 * PURPOSE:
 *   Calculate and commit bounded resize geometry while leaving pointer handling
 *   in frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/resize.h"
#include "internal.h"
#include <math.h>


/*
 * Provide the workbench designer resize policy default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerResizePolicy umi_workbench_designer_resize_policy_default(void)
{
    UmiWorkbenchDesignerResizePolicy policy;
    policy.minimum_size.width = 80.0;
    policy.minimum_size.height = 60.0;
    policy.maximum_size.width = 8192.0;
    policy.maximum_size.height = 8192.0;
    policy.preserve_aspect_ratio = false;
    policy.clamp_to_canvas = true;
    policy.handle_size = 8.0;
    return policy;
}

/*
 * Initialise workbench designer resize from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_resize_init(
    UmiWorkbenchDesignerResizeSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->handle = UMI_WORKBENCH_DESIGNER_RESIZE_NONE;
    session->policy = umi_workbench_designer_resize_policy_default();
}

/*
 * Provide the workbench designer resize begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_resize_begin(
    UmiWorkbenchDesignerResizeSession *session,
    const char *node_id,
    UmiWorkbenchDesignerResizeHandle handle,
    UmiWorkbenchDesignerPoint pointer,
    UmiWorkbenchDesignerRect bounds,
    const UmiWorkbenchDesignerResizePolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL || node_id[0] == '\0' ||
        handle <= UMI_WORKBENCH_DESIGNER_RESIZE_NONE ||
        handle > UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_WEST ||
        !umi_workbench_designer_rect_is_valid(&bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_resize_init(session);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_designer_copy_text(
            session->node_id, sizeof(session->node_id), node_id) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    session->handle = handle;
    session->start_point = pointer;
    session->original_bounds = bounds;
    session->preview_bounds = bounds;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy != NULL) session->policy = *policy;
    session->active = true;
    session->revision = 1U;
    return UMI_STATUS_OK;
}

/* Provide the resize has west operation used by this module and its client applications. */
static bool resize_has_west(UmiWorkbenchDesignerResizeHandle handle)
{
    return handle == UMI_WORKBENCH_DESIGNER_RESIZE_WEST ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_WEST ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_WEST;
}

/* Provide the resize has east operation used by this module and its client applications. */
static bool resize_has_east(UmiWorkbenchDesignerResizeHandle handle)
{
    return handle == UMI_WORKBENCH_DESIGNER_RESIZE_EAST ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_EAST ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_EAST;
}

/* Provide the resize has north operation used by this module and its client applications. */
static bool resize_has_north(UmiWorkbenchDesignerResizeHandle handle)
{
    return handle == UMI_WORKBENCH_DESIGNER_RESIZE_NORTH ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_EAST ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_WEST;
}

/* Provide the resize has south operation used by this module and its client applications. */
static bool resize_has_south(UmiWorkbenchDesignerResizeHandle handle)
{
    return handle == UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_EAST ||
        handle == UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_WEST;
}

/* Return the number of records represented by resize enforce without changing their state. */
static void resize_enforce_size(
    UmiWorkbenchDesignerRect *rect,
    const UmiWorkbenchDesignerResizePolicy *policy,
    UmiWorkbenchDesignerResizeHandle handle)
{
    const double old_right = rect->x + rect->width;
    const double old_bottom = rect->y + rect->height;
    rect->width = umi_workbench_designer_clamp(
        rect->width, policy->minimum_size.width, policy->maximum_size.width);
    rect->height = umi_workbench_designer_clamp(
        rect->height, policy->minimum_size.height, policy->maximum_size.height);
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_west(handle)) rect->x = old_right - rect->width;
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_north(handle)) rect->y = old_bottom - rect->height;
}

/*
 * Provide the resize enforce aspect operation used by this module and its client
 * applications.
 */
static void resize_enforce_aspect(
    UmiWorkbenchDesignerRect *rect,
    UmiWorkbenchDesignerRect original,
    UmiWorkbenchDesignerResizeHandle handle)
{
    double ratio;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (original.height <= 0.0) return;
    ratio = original.width / original.height;
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_east(handle) || resize_has_west(handle)) {
        const double old_bottom = rect->y + rect->height;
        rect->height = rect->width / ratio;
        /* Apply this branch only when its contract condition is satisfied. */
        if (resize_has_north(handle)) rect->y = old_bottom - rect->height;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        const double old_right = rect->x + rect->width;
        rect->width = rect->height * ratio;
        /* Apply this branch only when its contract condition is satisfied. */
        if (resize_has_west(handle)) rect->x = old_right - rect->width;
    }
}

/*
 * Provide the resize clamp canvas operation used by this module and its client
 * applications.
 */
static void resize_clamp_canvas(
    UmiWorkbenchDesignerRect *rect,
    UmiWorkbenchDesignerRect canvas)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_rect_is_valid(&canvas)) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->x < canvas.x) rect->x = canvas.x;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->y < canvas.y) rect->y = canvas.y;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->x + rect->width > canvas.x + canvas.width) {
        rect->width = canvas.x + canvas.width - rect->x;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->y + rect->height > canvas.y + canvas.height) {
        rect->height = canvas.y + canvas.height - rect->y;
    }
}

/*
 * Provide the workbench designer resize update operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_resize_update(
    UmiWorkbenchDesignerResizeSession *session,
    UmiWorkbenchDesignerPoint pointer,
    UmiWorkbenchDesignerRect canvas_bounds)
{
    UmiWorkbenchDesignerRect rect;
    const double dx = pointer.x - session->start_point.x;
    const double dy = pointer.y - session->start_point.y;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->active) return UMI_STATUS_INVALID_STATE;
    rect = session->original_bounds;
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_west(session->handle)) {
        rect.x += dx;
        rect.width -= dx;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_east(session->handle)) rect.width += dx;
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_north(session->handle)) {
        rect.y += dy;
        rect.height -= dy;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (resize_has_south(session->handle)) rect.height += dy;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->policy.preserve_aspect_ratio) {
        resize_enforce_aspect(&rect, session->original_bounds, session->handle);
    }
    resize_enforce_size(&rect, &session->policy, session->handle);
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->policy.clamp_to_canvas) {
        resize_clamp_canvas(&rect, canvas_bounds);
        resize_enforce_size(&rect, &session->policy, session->handle);
    }
    session->preview_bounds = rect;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer resize commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_resize_commit(
    UmiWorkbenchDesignerResizeSession *session,
    UmiWorkbenchLayoutDocument *document)
{
    UmiWorkbenchLayoutNode *node;
    UmiWorkbenchLayoutRect bounds;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || document == NULL || !session->active) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = umi_workbench_layout_document_find_node_mutable(
        document, session->node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    bounds = umi_workbench_designer_to_layout_rect(session->preview_bounds);
    status = umi_workbench_layout_node_set_bounds(node, &bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    node->revision += 1U;
    umi_workbench_layout_document_increment_revision(document);
    umi_workbench_layout_document_refresh_hash(document);
    session->active = false;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer resize cancel operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_resize_cancel(
    UmiWorkbenchDesignerResizeSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    session->preview_bounds = session->original_bounds;
    session->active = false;
    session->revision += 1U;
}

/*
 * Provide the workbench designer resize hit test operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerResizeHandle umi_workbench_designer_resize_hit_test(
    UmiWorkbenchDesignerRect bounds,
    UmiWorkbenchDesignerPoint point,
    double handle_size)
{
    const double left = fabs(point.x - bounds.x);
    const double right = fabs(point.x - (bounds.x + bounds.width));
    const double top = fabs(point.y - bounds.y);
    const double bottom = fabs(point.y - (bounds.y + bounds.height));
    const bool near_left = left <= handle_size;
    const bool near_right = right <= handle_size;
    const bool near_top = top <= handle_size;
    const bool near_bottom = bottom <= handle_size;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_designer_rect_contains_point(&bounds, point) &&
        !(near_left || near_right || near_top || near_bottom)) {
        return UMI_WORKBENCH_DESIGNER_RESIZE_NONE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_top && near_left) return UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_WEST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_top && near_right) return UMI_WORKBENCH_DESIGNER_RESIZE_NORTH_EAST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_bottom && near_left) return UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_WEST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_bottom && near_right) return UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_EAST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_top) return UMI_WORKBENCH_DESIGNER_RESIZE_NORTH;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_right) return UMI_WORKBENCH_DESIGNER_RESIZE_EAST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_bottom) return UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH;
    /* Apply this branch only when its contract condition is satisfied. */
    if (near_left) return UMI_WORKBENCH_DESIGNER_RESIZE_WEST;
    return UMI_WORKBENCH_DESIGNER_RESIZE_NONE;
}
