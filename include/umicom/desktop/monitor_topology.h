/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/monitor_topology.h
 *
 * PURPOSE:
 *   Publish the public monitor topology contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_MONITOR_TOPOLOGY_H
#define UMICOM_DESKTOP_MONITOR_TOPOLOGY_H

#include "umicom/base/status.h"
#include "umicom/desktop/types.h"

typedef struct UmiDesktopMonitorTopology {
    UmiDesktopMonitor monitors[UMI_DESKTOP_MAX_MONITORS];
    size_t count;
    uint64_t revision;
} UmiDesktopMonitorTopology;

void umi_desktop_monitor_topology_init(UmiDesktopMonitorTopology *topology);
UmiStatus umi_desktop_monitor_topology_add(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor);
UmiStatus umi_desktop_monitor_topology_update(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor);
UmiStatus umi_desktop_monitor_topology_remove(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id);
UmiStatus umi_desktop_monitor_topology_set_primary(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id);
const UmiDesktopMonitor *umi_desktop_monitor_topology_find(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id);
const UmiDesktopMonitor *umi_desktop_monitor_topology_primary(
    const UmiDesktopMonitorTopology *topology);
UmiStatus umi_desktop_monitor_topology_constrain(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id,
    UmiDesktopRect *bounds);

#endif
