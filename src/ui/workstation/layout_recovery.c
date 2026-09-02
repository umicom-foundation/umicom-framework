/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_recovery.c
 *
 * PURPOSE:
 *   Implement missing-monitor and invalid-geometry recovery for portable saved workstation layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/layout_recovery.h"
#include "umicom/ui/workstation/layout_geometry.h"

/*
 * Provide the ws layout recover window operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_recover_window(const UmiWsFloatingWindow *window,
                                       const UmiWsMonitorTopology *topology,
                                       UmiWsLayoutRecoveryResult *out_result) {
    const UmiWsMonitor *monitor;
    UmiUiRect recovered;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || topology == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    monitor = umi_ws_monitor_topology_find(topology, window->monitor_id);
    *out_result = (UmiWsLayoutRecoveryResult){0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) {
        monitor = umi_ws_monitor_topology_fallback(topology);
        out_result->monitor_changed = true;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) return UMI_STATUS_NOT_FOUND;
    recovered = umi_ws_layout_geometry_clamp(window->bounds, monitor->work_area);
    out_result->recovered_bounds = recovered;
    out_result->geometry_changed = recovered.x != window->bounds.x || recovered.y != window->bounds.y ||
                                   recovered.width != window->bounds.width || recovered.height != window->bounds.height;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(out_result->chosen_monitor_id, sizeof(out_result->chosen_monitor_id), monitor->monitor_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
