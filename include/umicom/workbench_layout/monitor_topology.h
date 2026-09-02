/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/monitor_topology.h
 *
 * PURPOSE:
 *   Represent monitor work areas, scale, primary-monitor selection and topology changes without depending on GTK, Win32 or Wayland types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_MONITOR_TOPOLOGY_H
#define UMICOM_WORKBENCH_LAYOUT_MONITOR_TOPOLOGY_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench monitor data shared with callers of this public contract.
 */
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

/**
 * Represent the workbench monitor topology data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchMonitorTopology {
    uint32_t structure_size;
    UmiWorkbenchMonitor monitors[UMI_WORKBENCH_LAYOUT_MAX_MONITORS];
    size_t count;
    char primary_monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchMonitorTopology;

/**
 * Initialise workbench monitor topology from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_monitor_topology_init(
    UmiWorkbenchMonitorTopology *topology);

/**
 * Check that workbench monitor satisfies its contract before another service relies on it.
 */
UmiStatus umi_workbench_monitor_validate(
    const UmiWorkbenchMonitor *monitor);

/**
 * Add workbench monitor topology only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_monitor_topology_add(
    UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchMonitor *monitor);

/**
 * Remove workbench monitor topology while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_monitor_topology_remove(
    UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id);

/**
 * Provide the workbench monitor topology set primary operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_monitor_topology_set_primary(
    UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id);

/**
 * Find workbench monitor topology while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_find(
    const UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id);

/**
 * Provide the workbench monitor topology primary operation used by this module and its
 * client applications.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_primary(
    const UmiWorkbenchMonitorTopology *topology);

/**
 * Find workbench monitor topology while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_at(
    const UmiWorkbenchMonitorTopology *topology,
    size_t index);

/**
 * Provide the workbench monitor topology for point operation used by this module and its
 * client applications.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_for_point(
    const UmiWorkbenchMonitorTopology *topology,
    UmiWorkbenchLayoutPoint point);

/**
 * Provide the workbench monitor topology best for rect operation used by this module and
 * its client applications.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_best_for_rect(
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutRect *rect);

#ifdef __cplusplus
}
#endif

#endif
