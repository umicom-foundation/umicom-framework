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

typedef struct UmiWsMonitor {
    char monitor_id[UMI_UI_ID_CAPACITY];
    UmiUiRect work_area;
    int32_t scale_percent;
    bool primary;
} UmiWsMonitor;

typedef struct UmiWsMonitorTopology {
    UmiWsMonitor monitors[UMI_WS_MAX_MONITORS];
    size_t count;
} UmiWsMonitorTopology;

void umi_ws_monitor_topology_init(UmiWsMonitorTopology *topology);
UmiStatus umi_ws_monitor_topology_add(UmiWsMonitorTopology *topology, const UmiWsMonitor *monitor);
const UmiWsMonitor *umi_ws_monitor_topology_find(const UmiWsMonitorTopology *topology, const char *monitor_id);
const UmiWsMonitor *umi_ws_monitor_topology_fallback(const UmiWsMonitorTopology *topology);

#ifdef __cplusplus
}
#endif

#endif
