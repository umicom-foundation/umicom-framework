/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/monitor_topology.c
 *
 * PURPOSE:
 *   Implement monitor topology and deterministic fallback selection for portable multi-monitor workstation layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/monitor_topology.h"

/*
 * Initialise ws monitor topology from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_monitor_topology_init(UmiWsMonitorTopology *topology) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology != NULL) *topology = (UmiWsMonitorTopology){0};
}

/* Add ws monitor topology only after its inputs and available capacity have been checked. */
UmiStatus umi_ws_monitor_topology_add(UmiWsMonitorTopology *topology, const UmiWsMonitor *monitor) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || monitor == NULL || !umi_ws_id_valid(monitor->monitor_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (monitor->work_area.width <= 0 || monitor->work_area.height <= 0 || monitor->scale_percent <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(topology->monitors[index].monitor_id, monitor->monitor_id) == 0) {
            topology->monitors[index] = *monitor;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (topology->count >= UMI_WS_MAX_MONITORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    topology->monitors[topology->count++] = *monitor;
    return UMI_STATUS_OK;
}

/*
 * Find ws monitor topology while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsMonitor *umi_ws_monitor_topology_find(const UmiWsMonitorTopology *topology, const char *monitor_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || !umi_ws_id_valid(monitor_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(topology->monitors[index].monitor_id, monitor_id) == 0) return &topology->monitors[index];
    }
    return NULL;
}

/*
 * Provide the ws monitor topology fallback operation used by this module and its client
 * applications.
 */
const UmiWsMonitor *umi_ws_monitor_topology_fallback(const UmiWsMonitorTopology *topology) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || topology->count == 0U) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (topology->monitors[index].primary) return &topology->monitors[index];
    }
    return &topology->monitors[0];
}
