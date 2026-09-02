/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/monitor_topology.h
 *
 * PURPOSE:
 *   Define monitor topology and deterministic fallback selection for portable multi-monitor workstation layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_MONITOR_TOPOLOGY_H
#define UMICOM_UI_WORKSTATION_MONITOR_TOPOLOGY_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws monitor data shared with callers of this public contract.
 */
typedef struct UmiWsMonitor {
    char monitor_id[UMI_UI_ID_CAPACITY];
    UmiUiRect work_area;
    int32_t scale_percent;
    bool primary;
} UmiWsMonitor;

/**
 * Represent the ws monitor topology data shared with callers of this public contract.
 */
typedef struct UmiWsMonitorTopology {
    UmiWsMonitor monitors[UMI_WS_MAX_MONITORS];
    size_t count;
} UmiWsMonitorTopology;

/**
 * Initialise ws monitor topology from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_monitor_topology_init(UmiWsMonitorTopology *topology);
/**
 * Add ws monitor topology only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ws_monitor_topology_add(UmiWsMonitorTopology *topology, const UmiWsMonitor *monitor);
/**
 * Find ws monitor topology while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsMonitor *umi_ws_monitor_topology_find(const UmiWsMonitorTopology *topology, const char *monitor_id);
/**
 * Provide the ws monitor topology fallback operation used by this module and its client
 * applications.
 */
const UmiWsMonitor *umi_ws_monitor_topology_fallback(const UmiWsMonitorTopology *topology);

#ifdef __cplusplus
}
#endif

#endif
