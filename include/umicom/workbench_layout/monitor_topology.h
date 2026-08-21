/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/monitor_topology.h
 *
 * PURPOSE:
 *   Represent monitor work areas, scale, primary-monitor selection and topology changes without depending on GTK, Win32 or Wayland types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_MONITOR_TOPOLOGY_H
#define UMICOM_WORKBENCH_LAYOUT_MONITOR_TOPOLOGY_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchMonitor {
    uint32_t structure_size;
    char monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    UmiWorkbenchLayoutRect bounds;
    UmiWorkbenchLayoutRect work_area;
    double scale;
    double refresh_rate_hz;
    bool primary;
    bool enabled;
    uint64_t revision;
} UmiWorkbenchMonitor;

typedef struct UmiWorkbenchMonitorTopology {
    uint32_t structure_size;
    UmiWorkbenchMonitor monitors[UMI_WORKBENCH_LAYOUT_MAX_MONITORS];
    size_t count;
    char primary_monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchMonitorTopology;

void umi_workbench_monitor_topology_init(
    UmiWorkbenchMonitorTopology *topology);

UmiStatus umi_workbench_monitor_validate(
    const UmiWorkbenchMonitor *monitor);

UmiStatus umi_workbench_monitor_topology_add(
    UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchMonitor *monitor);

UmiStatus umi_workbench_monitor_topology_remove(
    UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id);

UmiStatus umi_workbench_monitor_topology_set_primary(
    UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id);

const UmiWorkbenchMonitor *umi_workbench_monitor_topology_find(
    const UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id);

const UmiWorkbenchMonitor *umi_workbench_monitor_topology_primary(
    const UmiWorkbenchMonitorTopology *topology);

const UmiWorkbenchMonitor *umi_workbench_monitor_topology_at(
    const UmiWorkbenchMonitorTopology *topology,
    size_t index);

const UmiWorkbenchMonitor *umi_workbench_monitor_topology_for_point(
    const UmiWorkbenchMonitorTopology *topology,
    UmiWorkbenchLayoutPoint point);

const UmiWorkbenchMonitor *umi_workbench_monitor_topology_best_for_rect(
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutRect *rect);

#ifdef __cplusplus
}
#endif

#endif
