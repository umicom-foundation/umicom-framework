/* Umicom Framework Tests | Desktop window manager | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/window_manager.h"

int main(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopWindowManager manager;
    UmiDesktopMonitor monitor = {0};
    UmiDesktopWindow window = {0};
    (void)snprintf(monitor.monitor_id, sizeof(monitor.monitor_id), "primary");
    (void)snprintf(monitor.name, sizeof(monitor.name), "Primary");
    monitor.bounds = (UmiDesktopRect){0, 0, 1920, 1080};
    monitor.work_area = (UmiDesktopRect){0, 0, 1920, 1040};
    monitor.scale = 1.0;
    monitor.refresh_rate_hz = 60.0;
    monitor.primary = true;
    monitor.enabled = true;
    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(&topology, &monitor) == UMI_STATUS_OK);
    umi_desktop_window_manager_init(&manager);
    (void)snprintf(window.window_id, sizeof(window.window_id), "chart-one");
    (void)snprintf(window.title, sizeof(window.title), "Advanced Chart");
    (void)snprintf(window.component_id, sizeof(window.component_id),
                   "umicom.trading.chart");
    (void)snprintf(window.owner_application_id,
                   sizeof(window.owner_application_id), "org.umicom.trader");
    (void)snprintf(window.monitor_id, sizeof(window.monitor_id), "primary");
    window.bounds = (UmiDesktopRect){1700, 900, 800, 600};
    window.dock_placement = UMI_DESKTOP_DOCK_FLOATING;
    window.visible = true;
    window.closable = true;
    window.resizable = true;
    assert(umi_desktop_window_manager_open(&manager, &topology, &window) ==
           UMI_STATUS_OK);
    assert(manager.windows[0].bounds.x == 1120);
    assert(manager.windows[0].bounds.y == 440);
    assert(umi_desktop_window_manager_place(
        &manager, &topology, "chart-one", "primary",
        (UmiDesktopRect){0, 0, 960, 1040}, UMI_DESKTOP_DOCK_LEFT) == UMI_STATUS_OK);
    assert(manager.windows[0].dock_placement == UMI_DESKTOP_DOCK_LEFT);
    assert(umi_desktop_window_manager_maximise(&manager, "chart-one", true) ==
           UMI_STATUS_OK);
    assert(umi_desktop_window_manager_close(&manager, "chart-one") == UMI_STATUS_OK);
    assert(manager.count == 0U);
    return 0;
}
