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

/**
 * Represent the desktop monitor topology data shared with callers of this public contract.
 */
typedef struct UmiDesktopMonitorTopology {
    UmiDesktopMonitor monitors[UMI_DESKTOP_MAX_MONITORS];
    size_t count;
    uint64_t revision;
} UmiDesktopMonitorTopology;

/**
 * Initialise desktop monitor topology from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_monitor_topology_init(UmiDesktopMonitorTopology *topology);
/**
 * Add desktop monitor topology only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_desktop_monitor_topology_add(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor);
/**
 * Provide the desktop monitor topology update operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_monitor_topology_update(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor);
/**
 * Remove desktop monitor topology while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_desktop_monitor_topology_remove(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id);
/**
 * Provide the desktop monitor topology set primary operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_topology_set_primary(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id);
/**
 * Find desktop monitor topology while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesktopMonitor *umi_desktop_monitor_topology_find(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id);
/**
 * Provide the desktop monitor topology primary operation used by this module and its
 * client applications.
 */
const UmiDesktopMonitor *umi_desktop_monitor_topology_primary(
    const UmiDesktopMonitorTopology *topology);
/**
 * Provide the desktop monitor topology constrain operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_topology_constrain(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id,
    UmiDesktopRect *bounds);

#endif
