/*-----------------------------------------------------------------------------
 * Umicom Framework | Desktop monitor topology
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/monitor_topology.h"

#include <string.h>

static size_t find_monitor_index(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    if (topology == NULL || monitor_id == NULL) return SIZE_MAX;
    for (index = 0U; index < topology->count; ++index) {
        if (strcmp(topology->monitors[index].monitor_id, monitor_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

static bool valid_rect(UmiDesktopRect rect)
{
    return rect.width > 0 && rect.height > 0;
}

static bool valid_monitor(const UmiDesktopMonitor *monitor)
{
    return monitor != NULL && monitor->monitor_id[0] != '\0' &&
        monitor->name[0] != '\0' && valid_rect(monitor->bounds) &&
        valid_rect(monitor->work_area) && monitor->scale > 0.0 &&
        monitor->refresh_rate_hz > 0.0;
}

void umi_desktop_monitor_topology_init(UmiDesktopMonitorTopology *topology)
{
    if (topology == NULL) return;
    (void)memset(topology, 0, sizeof(*topology));
}

UmiStatus umi_desktop_monitor_topology_add(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor)
{
    size_t index;
    if (topology == NULL || !valid_monitor(monitor))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (find_monitor_index(topology, monitor->monitor_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    if (topology->count >= UMI_DESKTOP_MAX_MONITORS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (monitor->primary) {
        for (index = 0U; index < topology->count; ++index)
            topology->monitors[index].primary = false;
    }
    topology->monitors[topology->count++] = *monitor;
    if (topology->count == 1U) topology->monitors[0].primary = true;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_topology_update(
    UmiDesktopMonitorTopology *topology,
    const UmiDesktopMonitor *monitor)
{
    size_t index;
    size_t current;
    if (topology == NULL || !valid_monitor(monitor))
        return UMI_STATUS_INVALID_ARGUMENT;
    current = find_monitor_index(topology, monitor->monitor_id);
    if (current == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (monitor->primary) {
        for (index = 0U; index < topology->count; ++index)
            topology->monitors[index].primary = false;
    }
    topology->monitors[current] = *monitor;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_topology_remove(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    bool removed_primary;
    if (topology == NULL || monitor_id == NULL || monitor_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_monitor_index(topology, monitor_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    removed_primary = topology->monitors[index].primary;
    if (index + 1U < topology->count) {
        (void)memmove(&topology->monitors[index],
                      &topology->monitors[index + 1U],
                      (topology->count - index - 1U) *
                          sizeof(topology->monitors[0]));
    }
    topology->count -= 1U;
    (void)memset(&topology->monitors[topology->count], 0,
                 sizeof(topology->monitors[0]));
    if (removed_primary && topology->count > 0U)
        topology->monitors[0].primary = true;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_topology_set_primary(
    UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    size_t selected;
    if (topology == NULL || monitor_id == NULL || monitor_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    selected = find_monitor_index(topology, monitor_id);
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < topology->count; ++index)
        topology->monitors[index].primary = index == selected;
    topology->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiDesktopMonitor *umi_desktop_monitor_topology_find(
    const UmiDesktopMonitorTopology *topology,
    const char *monitor_id)
{
    const size_t index = find_monitor_index(topology, monitor_id);
    return index != SIZE_MAX ? &topology->monitors[index] : NULL;
}

const UmiDesktopMonitor *umi_desktop_monitor_topology_primary(
    const UmiDesktopMonitorTopology *topology)
{
    size_t index;
    if (topology == NULL) return NULL;
    for (index = 0U; index < topology->count; ++index) {
        if (topology->monitors[index].primary &&
            topology->monitors[index].enabled)
            return &topology->monitors[index];
    }
    return topology->count > 0U ? &topology->monitors[0] : NULL;
}

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
    if (topology == NULL || bounds == NULL || bounds->width <= 0 ||
        bounds->height <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    monitor = umi_desktop_monitor_topology_find(topology, monitor_id);
    if (monitor == NULL || !monitor->enabled) return UMI_STATUS_NOT_FOUND;
    if (bounds->width > monitor->work_area.width)
        bounds->width = monitor->work_area.width;
    if (bounds->height > monitor->work_area.height)
        bounds->height = monitor->work_area.height;
    if (bounds->x < monitor->work_area.x) bounds->x = monitor->work_area.x;
    if (bounds->y < monitor->work_area.y) bounds->y = monitor->work_area.y;
    right = (int64_t)bounds->x + (int64_t)bounds->width;
    bottom = (int64_t)bounds->y + (int64_t)bounds->height;
    work_right = (int64_t)monitor->work_area.x +
        (int64_t)monitor->work_area.width;
    work_bottom = (int64_t)monitor->work_area.y +
        (int64_t)monitor->work_area.height;
    if (right > work_right)
        bounds->x = (int32_t)(work_right - (int64_t)bounds->width);
    if (bottom > work_bottom)
        bounds->y = (int32_t)(work_bottom - (int64_t)bounds->height);
    return UMI_STATUS_OK;
}
