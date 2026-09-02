/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/monitor_canvas.c
 *
 * PURPOSE:
 *   Implement one deterministic projection for monitor-manager previews,
 *   layout-designer canvases and future remote/web desktop presentations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/monitor_canvas.h"

#include <float.h>
#include <stdio.h>
#include <string.h>

/* Provide the project rect operation used by this module and its client applications. */
static UmiDesktopCanvasRect project_rect(
    const UmiDesktopMonitorCanvas *canvas,
    UmiDesktopRect source)
{
    UmiDesktopCanvasRect result;
    result.x = canvas->offset_x +
        ((double)source.x - (double)canvas->desktop_bounds.x) * canvas->scale;
    result.y = canvas->offset_y +
        ((double)source.y - (double)canvas->desktop_bounds.y) * canvas->scale;
    result.width = (double)source.width * canvas->scale;
    result.height = (double)source.height * canvas->scale;
    return result;
}

/* Provide the rounded delta operation used by this module and its client applications. */
static int32_t rounded_delta(double value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 0.0) return (int32_t)(value + 0.5);
    return (int32_t)(value - 0.5);
}

/*
 * Provide the desktop monitor canvas build operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_monitor_canvas_build(
    const UmiDesktopMonitorTopology *topology,
    double viewport_width,
    double viewport_height,
    double padding,
    UmiDesktopMonitorCanvas *out_canvas)
{
    int64_t left = INT32_MAX;
    int64_t top = INT32_MAX;
    int64_t right = INT32_MIN;
    int64_t bottom = INT32_MIN;
    double available_width;
    double available_height;
    double scale_x;
    double scale_y;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || out_canvas == NULL || topology->count == 0U ||
        viewport_width <= 0.0 || viewport_height <= 0.0 || padding < 0.0 ||
        viewport_width <= padding * 2.0 || viewport_height <= padding * 2.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        const UmiDesktopMonitor *monitor = &topology->monitors[index];
        int64_t monitor_right;
        int64_t monitor_bottom;
        /* Apply this operation only while the related capability or state is available. */
        if (!monitor->enabled) continue;
        monitor_right = (int64_t)monitor->bounds.x + monitor->bounds.width;
        monitor_bottom = (int64_t)monitor->bounds.y + monitor->bounds.height;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((int64_t)monitor->bounds.x < left) left = monitor->bounds.x;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((int64_t)monitor->bounds.y < top) top = monitor->bounds.y;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor_right > right) right = monitor_right;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor_bottom > bottom) bottom = monitor_bottom;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (left == INT32_MAX || right <= left || bottom <= top)
        return UMI_STATUS_INVALID_STATE;
    (void)memset(out_canvas, 0, sizeof(*out_canvas));
    out_canvas->desktop_bounds.x = (int32_t)left;
    out_canvas->desktop_bounds.y = (int32_t)top;
    out_canvas->desktop_bounds.width = (int32_t)(right - left);
    out_canvas->desktop_bounds.height = (int32_t)(bottom - top);
    out_canvas->viewport = (UmiDesktopCanvasRect){
        0.0, 0.0, viewport_width, viewport_height};
    out_canvas->padding = padding;
    available_width = viewport_width - padding * 2.0;
    available_height = viewport_height - padding * 2.0;
    scale_x = available_width / (double)out_canvas->desktop_bounds.width;
    scale_y = available_height / (double)out_canvas->desktop_bounds.height;
    out_canvas->scale = scale_x < scale_y ? scale_x : scale_y;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_canvas->scale <= DBL_EPSILON) return UMI_STATUS_INVALID_STATE;
    out_canvas->offset_x = padding + (available_width -
        (double)out_canvas->desktop_bounds.width * out_canvas->scale) / 2.0;
    out_canvas->offset_y = padding + (available_height -
        (double)out_canvas->desktop_bounds.height * out_canvas->scale) / 2.0;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop monitor canvas project monitor operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_canvas_project_monitor(
    const UmiDesktopMonitorCanvas *canvas,
    const UmiDesktopMonitor *monitor,
    UmiDesktopCanvasMonitor *out_monitor)
{
    int first;
    int second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || monitor == NULL || out_monitor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_monitor, 0, sizeof(*out_monitor));
    first = snprintf(out_monitor->monitor_id, sizeof(out_monitor->monitor_id),
                     "%s", monitor->monitor_id);
    second = snprintf(out_monitor->name, sizeof(out_monitor->name), "%s",
                      monitor->name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0 || second < 0 ||
        (size_t)first >= sizeof(out_monitor->monitor_id) ||
        (size_t)second >= sizeof(out_monitor->name))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_monitor->bounds = project_rect(canvas, monitor->bounds);
    out_monitor->work_area = project_rect(canvas, monitor->work_area);
    out_monitor->primary = monitor->primary;
    out_monitor->enabled = monitor->enabled;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop monitor canvas project window operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_canvas_project_window(
    const UmiDesktopMonitorCanvas *canvas,
    const UmiDesktopWindow *window,
    bool selected,
    bool locked,
    UmiDesktopCanvasWindow *out_window)
{
    int first;
    int second;
    int third;
    int fourth = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || window == NULL || out_window == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_window, 0, sizeof(*out_window));
    first = snprintf(out_window->window_id, sizeof(out_window->window_id),
                     "%s", window->window_id);
    second = snprintf(out_window->title, sizeof(out_window->title), "%s",
                      window->title);
    third = snprintf(out_window->owner_application_id,
                     sizeof(out_window->owner_application_id), "%s",
                     window->owner_application_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (window->context_group_id[0] != '\0') {
        fourth = snprintf(out_window->context_group_id,
                          sizeof(out_window->context_group_id), "%s",
                          window->context_group_id);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0 || second < 0 || third < 0 || fourth < 0 ||
        (size_t)first >= sizeof(out_window->window_id) ||
        (size_t)second >= sizeof(out_window->title) ||
        (size_t)third >= sizeof(out_window->owner_application_id) ||
        (size_t)fourth >= sizeof(out_window->context_group_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_window->bounds = project_rect(canvas, window->bounds);
    out_window->dock_placement = window->dock_placement;
    out_window->z_order = window->z_order;
    out_window->visible = window->visible;
    out_window->selected = selected;
    out_window->locked = locked;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop monitor canvas delta to desktop operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_monitor_canvas_delta_to_desktop(
    const UmiDesktopMonitorCanvas *canvas,
    double canvas_delta_x,
    double canvas_delta_y,
    int32_t *out_desktop_delta_x,
    int32_t *out_desktop_delta_y)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL || out_desktop_delta_x == NULL ||
        out_desktop_delta_y == NULL || canvas->scale <= DBL_EPSILON)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_desktop_delta_x = rounded_delta(canvas_delta_x / canvas->scale);
    *out_desktop_delta_y = rounded_delta(canvas_delta_y / canvas->scale);
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop canvas rect contains operation used by this module and its client
 * applications.
 */
bool umi_desktop_canvas_rect_contains(
    UmiDesktopCanvasRect rect,
    double x,
    double y)
{
    return x >= rect.x && y >= rect.y &&
        x <= rect.x + rect.width && y <= rect.y + rect.height;
}
