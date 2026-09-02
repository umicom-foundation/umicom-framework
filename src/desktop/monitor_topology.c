/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/monitor_topology.c
 *
 * PURPOSE:
 *   Implement the monitor topology behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Desktop monitor topology
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/monitor_topology.h"

#include <string.h>

/*
 * Provide the find monitor index operation used by this module and its client
 * applications.
 */
static size_t find_monitor_index(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || monitor_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(topology->monitors[index].monitor_id, monitor_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

/* Provide the valid rect operation used by this module and its client applications. */
static bool valid_rect(UmiDesktopRect rect)
{
    return rect.width > 0 && rect.height > 0;
}

/* Provide the valid monitor operation used by this module and its client applications. */
static bool valid_monitor(const UmiDesktopMonitor *monitor)
{
    return monitor != NULL && monitor->monitor_id[0] != '\0' &&
        monitor->name[0] != '\0' && valid_rect(monitor->bounds) &&
        valid_rect(monitor->work_area) && monitor->scale > 0.0 &&
        monitor->refresh_rate_hz > 0.0;
}

/*
 * Initialise desktop monitor topology from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_monitor_topology_init(UmiDesktopMonitorTopology *topology)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL) return;
    (void)memset(topology, 0, sizeof(*topology));
}

/*
 * Add desktop monitor topology only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_desktop_monitor_topology_add(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || !valid_monitor(monitor))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_monitor_index(topology, monitor->monitor_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (topology->count >= UMI_DESKTOP_MAX_MONITORS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (monitor->primary) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < topology->count; ++index)
            topology->monitors[index].primary = false;
    }
    topology->monitors[topology->count++] = *monitor;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (topology->count == 1U) topology->monitors[0].primary = true;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop monitor topology update operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_monitor_topology_update(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor)
{
    size_t index;
    size_t current;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || !valid_monitor(monitor))
        return UMI_STATUS_INVALID_ARGUMENT;
    current = find_monitor_index(topology, monitor->monitor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (current == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (monitor->primary) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < topology->count; ++index)
            topology->monitors[index].primary = false;
    }
    topology->monitors[current] = *monitor;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove desktop monitor topology while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_desktop_monitor_topology_remove(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    bool removed_primary;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || monitor_id == NULL || monitor_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_monitor_index(topology, monitor_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    removed_primary = topology->monitors[index].primary;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < topology->count) {
        (void)memmove(&topology->monitors[index],
                      &topology->monitors[index + 1U],
                      (topology->count - index - 1U) *
                          sizeof(topology->monitors[0]));
    }
    topology->count -= 1U;
    (void)memset(&topology->monitors[topology->count], 0,
                 sizeof(topology->monitors[0]));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (removed_primary && topology->count > 0U)
        topology->monitors[0].primary = true;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop monitor topology set primary operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_topology_set_primary(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    size_t selected;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || monitor_id == NULL || monitor_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    selected = find_monitor_index(topology, monitor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index)
        topology->monitors[index].primary = index == selected;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find desktop monitor topology while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesktopMonitor *umi_desktop_monitor_topology_find(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    const size_t index = find_monitor_index(topology, monitor_id);
    return index != SIZE_MAX ? &topology->monitors[index] : NULL;
}

/*
 * Provide the desktop monitor topology primary operation used by this module and its
 * client applications.
 */
const UmiDesktopMonitor *umi_desktop_monitor_topology_primary(
    const UmiDesktopMonitorTopology *topology)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (topology->monitors[index].primary &&
            topology->monitors[index].enabled)
            return &topology->monitors[index];
    }
    return topology->count > 0U ? &topology->monitors[0] : NULL;
}

/*
 * Provide the desktop monitor topology constrain operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_topology_constrain(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id,
    UmiDesktopRect *bounds)
{
    const UmiDesktopMonitor *monitor;
    int64_t right;
    int64_t bottom;
    int64_t work_right;
    int64_t work_bottom;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || bounds == NULL || bounds->width <= 0 ||
        bounds->height <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    monitor = umi_desktop_monitor_topology_find(topology, monitor_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL || !monitor->enabled) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bounds->width > monitor->work_area.width)
        bounds->width = monitor->work_area.width;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bounds->height > monitor->work_area.height)
        bounds->height = monitor->work_area.height;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bounds->x < monitor->work_area.x) bounds->x = monitor->work_area.x;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bounds->y < monitor->work_area.y) bounds->y = monitor->work_area.y;
    right = (int64_t)bounds->x + (int64_t)bounds->width;
    bottom = (int64_t)bounds->y + (int64_t)bounds->height;
    work_right = (int64_t)monitor->work_area.x +
        (int64_t)monitor->work_area.width;
    work_bottom = (int64_t)monitor->work_area.y +
        (int64_t)monitor->work_area.height;
    /* Apply this branch only when its contract condition is satisfied. */
    if (right > work_right)
        bounds->x = (int32_t)(work_right - (int64_t)bounds->width);
    /* Apply this branch only when its contract condition is satisfied. */
    if (bottom > work_bottom)
        bounds->y = (int32_t)(work_bottom - (int64_t)bounds->height);
    return UMI_STATUS_OK;
}
