/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/monitor_topology.c
 *
 * PURPOSE:
 *   Implement monitor topology and deterministic fallback selection for portable multi-monitor workstation layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/monitor_topology.h"

void umi_ws_monitor_topology_init(UmiWsMonitorTopology *topology) {
    if (topology != NULL) *topology = (UmiWsMonitorTopology){0};
}

UmiStatus umi_ws_monitor_topology_add(UmiWsMonitorTopology *topology, const UmiWsMonitor *monitor) {
    size_t index;
    if (topology == NULL || monitor == NULL || !umi_ws_id_valid(monitor->monitor_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (monitor->work_area.width <= 0 || monitor->work_area.height <= 0 || monitor->scale_percent <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < topology->count; ++index) {
        if (strcmp(topology->monitors[index].monitor_id, monitor->monitor_id) == 0) {
            topology->monitors[index] = *monitor;
            return UMI_STATUS_OK;
        }
    }
    if (topology->count >= UMI_WS_MAX_MONITORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    topology->monitors[topology->count++] = *monitor;
    return UMI_STATUS_OK;
}

const UmiWsMonitor *umi_ws_monitor_topology_find(const UmiWsMonitorTopology *topology, const char *monitor_id) {
    size_t index;
    if (topology == NULL || !umi_ws_id_valid(monitor_id)) return NULL;
    for (index = 0U; index < topology->count; ++index) {
        if (strcmp(topology->monitors[index].monitor_id, monitor_id) == 0) return &topology->monitors[index];
    }
    return NULL;
}

const UmiWsMonitor *umi_ws_monitor_topology_fallback(const UmiWsMonitorTopology *topology) {
    size_t index;
    if (topology == NULL || topology->count == 0U) return NULL;
    for (index = 0U; index < topology->count; ++index) {
        if (topology->monitors[index].primary) return &topology->monitors[index];
    }
    return &topology->monitors[0];
}
