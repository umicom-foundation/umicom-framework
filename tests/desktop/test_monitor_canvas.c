/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_monitor_canvas.c
 *
 * PURPOSE:
 *   Implement the test monitor canvas behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop monitor canvas | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/monitor_canvas.h"

/*
 * Exercise monitor and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiDesktopMonitor monitor(const char *id, int32_t x, int32_t width,
                                 bool primary)
{
    UmiDesktopMonitor value = {0};
    (void)snprintf(value.monitor_id, sizeof(value.monitor_id), "%s", id);
    (void)snprintf(value.name, sizeof(value.name), "%s Display", id);
    value.bounds = (UmiDesktopRect){x, 0, width, 1080};
    value.work_area = (UmiDesktopRect){x, 0, width, 1040};
    value.scale = 1.0;
    value.refresh_rate_hz = 60.0;
    value.primary = primary;
    value.enabled = true;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor first = monitor("primary", 0, 1920, true);
    UmiDesktopMonitor second = monitor("secondary", 1920, 2560, false);
    UmiDesktopMonitorCanvas canvas;
    UmiDesktopCanvasMonitor projected;
    UmiDesktopWindow window = {0};
    UmiDesktopCanvasWindow projected_window;
    int32_t dx;
    int32_t dy;
    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(&topology, &first) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_topology_add(&topology, &second) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_canvas_build(
        &topology, 1000.0, 400.0, 20.0, &canvas) == UMI_STATUS_OK);
    assert(canvas.desktop_bounds.width == 4480);
    assert(canvas.scale > 0.20 && canvas.scale < 0.22);
    assert(umi_desktop_monitor_canvas_project_monitor(
        &canvas, &second, &projected) == UMI_STATUS_OK);
    assert(strcmp(projected.monitor_id, "secondary") == 0);
    assert(projected.bounds.x > 420.0);
    (void)snprintf(window.window_id, sizeof(window.window_id), "chart");
    (void)snprintf(window.title, sizeof(window.title), "Chart");
    (void)snprintf(window.owner_application_id,
                   sizeof(window.owner_application_id), "org.umicom.trader");
    window.bounds = (UmiDesktopRect){1920, 0, 1280, 1040};
    window.visible = true;
    assert(umi_desktop_monitor_canvas_project_window(
        &canvas, &window, true, false, &projected_window) == UMI_STATUS_OK);
    assert(projected_window.selected);
    assert(umi_desktop_canvas_rect_contains(
        projected_window.bounds, projected_window.bounds.x + 1.0,
        projected_window.bounds.y + 1.0));
    assert(umi_desktop_monitor_canvas_delta_to_desktop(
        &canvas, canvas.scale * 100.0, canvas.scale * -50.0,
        &dx, &dy) == UMI_STATUS_OK);
    assert(dx == 100 && dy == -50);
    return 0;
}
