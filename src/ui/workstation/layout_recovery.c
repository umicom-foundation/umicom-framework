/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_recovery.c
 *
 * PURPOSE:
 *   Implement missing-monitor and invalid-geometry recovery for portable saved workstation layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/layout_recovery.h"
#include "umicom/ui/workstation/layout_geometry.h"

UmiStatus umi_ws_layout_recover_window(const UmiWsFloatingWindow *window,
                                       const UmiWsMonitorTopology *topology,
                                       UmiWsLayoutRecoveryResult *out_result) {
    const UmiWsMonitor *monitor;
    UmiUiRect recovered;
    if (window == NULL || topology == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    monitor = umi_ws_monitor_topology_find(topology, window->monitor_id);
    *out_result = (UmiWsLayoutRecoveryResult){0};
    if (monitor == NULL) {
        monitor = umi_ws_monitor_topology_fallback(topology);
        out_result->monitor_changed = true;
    }
    if (monitor == NULL) return UMI_STATUS_NOT_FOUND;
    recovered = umi_ws_layout_geometry_clamp(window->bounds, monitor->work_area);
    out_result->recovered_bounds = recovered;
    out_result->geometry_changed = recovered.x != window->bounds.x || recovered.y != window->bounds.y ||
                                   recovered.width != window->bounds.width || recovered.height != window->bounds.height;
    if (umi_ws_copy_text(out_result->chosen_monitor_id, sizeof(out_result->chosen_monitor_id), monitor->monitor_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
