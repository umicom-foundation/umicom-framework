/* Umicom Framework Tests | Desktop persistence | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/persistence.h"

int main(void)
{
    UmiApplicationContextHub *first_hub = NULL;
    UmiApplicationContextHub *second_hub = NULL;
    UmiDesktopRuntime *first = NULL;
    UmiDesktopRuntime *second = NULL;
    UmiDesktopMonitor secondary = {0};
    UmiDesktopWindowManager *windows;
    const UmiDesktopWindow *chart;
    char text[32768U];
    size_t length = 0U;
    assert(umi_application_context_hub_create(&first_hub) == UMI_STATUS_OK);
    assert(umi_application_context_hub_create(&second_hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(first_hub, &first) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(second_hub, &second) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(first) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(second) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_activate_layout(first, "trading") == UMI_STATUS_OK);
    (void)snprintf(secondary.monitor_id, sizeof(secondary.monitor_id), "market-wall");
    (void)snprintf(secondary.name, sizeof(secondary.name), "Market Wall");
    secondary.bounds = (UmiDesktopRect){1920, 0, 2560, 1440};
    secondary.work_area = (UmiDesktopRect){1920, 0, 2560, 1400};
    secondary.scale = 1.25;
    secondary.refresh_rate_hz = 120.0;
    secondary.enabled = true;
    assert(umi_desktop_monitor_topology_add(
        umi_desktop_runtime_monitors(first), &secondary) == UMI_STATUS_OK);
    windows = umi_desktop_runtime_windows(first);
    assert(umi_desktop_window_manager_place(
        windows, umi_desktop_runtime_monitors(first), "trading-chart",
        "market-wall", (UmiDesktopRect){1920, 0, 1920, 1080},
        UMI_DESKTOP_DOCK_FLOATING) == UMI_STATUS_OK);
    assert(umi_desktop_persistence_encode(
        first, text, sizeof(text), &length) == UMI_STATUS_OK);
    assert(length > 1000U && strstr(text, "UMICOM-DESKTOP|1") != NULL);
    assert(umi_desktop_persistence_decode(second, text) == UMI_STATUS_OK);
    assert(strcmp(umi_desktop_runtime_layouts(second)->active_layout_id,
                  "trading") == 0);
    assert(umi_desktop_runtime_monitors(second)->count == 2U);
    chart = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(second), "trading-chart");
    assert(chart != NULL && strcmp(chart->monitor_id, "market-wall") == 0);
    assert(chart->dock_placement == UMI_DESKTOP_DOCK_FLOATING);
    umi_desktop_runtime_destroy(second);
    umi_desktop_runtime_destroy(first);
    umi_application_context_hub_destroy(second_hub);
    umi_application_context_hub_destroy(first_hub);
    return 0;
}
