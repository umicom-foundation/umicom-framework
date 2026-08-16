/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/monitor_interaction.c
 *
 * PURPOSE:
 *   Implement monitor-relative placement and safe evacuation when a display is
 *   disabled or disconnected. Protected built-in layouts remain immutable;
 *   reconciliation changes only their current session window state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/monitor_interaction.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDesktopMonitorInteraction {
    UmiDesktopRuntime *runtime;
    UmiDesktopMonitorAffinity affinities[UMI_DESKTOP_MONITOR_AFFINITY_MAX];
    char focused_monitor_id[UMI_DESKTOP_ID_CAPACITY];
    size_t affinity_count;
    size_t relocated_window_count;
    uint64_t revision;
};

static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static size_t find_affinity(
    const UmiDesktopMonitorInteraction *interaction,
    const char *window_id)
{
    size_t index;
    if (interaction == NULL || window_id == NULL) return SIZE_MAX;
    for (index = 0U; index < interaction->affinity_count; ++index) {
        if (strcmp(interaction->affinities[index].window_id,
                   window_id) == 0) return index;
    }
    return SIZE_MAX;
}

static UmiStatus remember_window(
    UmiDesktopMonitorInteraction *interaction,
    const UmiDesktopWindow *window)
{
    UmiDesktopMonitorAffinity *affinity;
    size_t index = find_affinity(interaction, window->window_id);
    UmiStatus status;
    if (index == SIZE_MAX) {
        if (interaction->affinity_count >= UMI_DESKTOP_MONITOR_AFFINITY_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = interaction->affinity_count++;
        (void)memset(&interaction->affinities[index], 0,
                     sizeof(interaction->affinities[index]));
    }
    affinity = &interaction->affinities[index];
    affinity->structure_size = (uint32_t)sizeof(*affinity);
    status = copy_text(affinity->window_id, sizeof(affinity->window_id),
                       window->window_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(affinity->monitor_id,
                           sizeof(affinity->monitor_id), window->monitor_id);
    if (status != UMI_STATUS_OK) return status;
    affinity->bounds = window->bounds;
    affinity->placement = window->dock_placement;
    affinity->revision += 1U;
    return UMI_STATUS_OK;
}

static size_t enabled_monitor_count(
    const UmiDesktopMonitorTopology *topology)
{
    size_t index;
    size_t count = 0U;
    if (topology == NULL) return 0U;
    for (index = 0U; index < topology->count; ++index) {
        if (topology->monitors[index].enabled) count += 1U;
    }
    return count;
}

UmiStatus umi_desktop_monitor_interaction_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopMonitorInteraction **out_interaction)
{
    UmiDesktopMonitorInteraction *interaction;
    const UmiDesktopMonitor *primary;
    UmiStatus status;
    if (runtime == NULL || out_interaction == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_interaction = NULL;
    primary = umi_desktop_monitor_topology_primary(
        umi_desktop_runtime_monitors(runtime));
    if (primary == NULL) return UMI_STATUS_INVALID_STATE;
    interaction = (UmiDesktopMonitorInteraction *)calloc(
        1U, sizeof(*interaction));
    if (interaction == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    interaction->runtime = runtime;
    status = copy_text(interaction->focused_monitor_id,
                       sizeof(interaction->focused_monitor_id),
                       primary->monitor_id);
    if (status != UMI_STATUS_OK) {
        free(interaction);
        return status;
    }
    interaction->revision = 1U;
    *out_interaction = interaction;
    return UMI_STATUS_OK;
}

void umi_desktop_monitor_interaction_destroy(
    UmiDesktopMonitorInteraction *interaction)
{
    free(interaction);
}

UmiStatus umi_desktop_monitor_interaction_focus(
    UmiDesktopMonitorInteraction *interaction,
    const char *monitor_id)
{
    const UmiDesktopMonitor *monitor;
    UmiStatus status;
    if (interaction == NULL || monitor_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    monitor = umi_desktop_monitor_topology_find(
        umi_desktop_runtime_monitors(interaction->runtime), monitor_id);
    if (monitor == NULL || !monitor->enabled) return UMI_STATUS_NOT_FOUND;
    status = copy_text(interaction->focused_monitor_id,
                       sizeof(interaction->focused_monitor_id), monitor_id);
    if (status == UMI_STATUS_OK) interaction->revision += 1U;
    return status;
}

UmiStatus umi_desktop_monitor_interaction_capture_affinities(
    UmiDesktopMonitorInteraction *interaction)
{
    UmiDesktopWindowManager *windows;
    size_t index;
    UmiStatus status;
    if (interaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    windows = umi_desktop_runtime_windows(interaction->runtime);
    for (index = 0U; index < windows->count; ++index) {
        status = remember_window(interaction, &windows->windows[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    interaction->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiDesktopRect map_bounds(
    UmiDesktopRect bounds,
    const UmiDesktopMonitor *source,
    const UmiDesktopMonitor *destination)
{
    UmiDesktopRect mapped = bounds;
    if (destination == NULL) return mapped;
    if (source != NULL && source->work_area.width > 0 &&
        source->work_area.height > 0) {
        mapped.x = destination->work_area.x + (int32_t)(
            (int64_t)(bounds.x - source->work_area.x) *
            destination->work_area.width / source->work_area.width);
        mapped.y = destination->work_area.y + (int32_t)(
            (int64_t)(bounds.y - source->work_area.y) *
            destination->work_area.height / source->work_area.height);
        mapped.width = (int32_t)(
            (int64_t)bounds.width * destination->work_area.width /
            source->work_area.width);
        mapped.height = (int32_t)(
            (int64_t)bounds.height * destination->work_area.height /
            source->work_area.height);
    } else {
        mapped.x = destination->work_area.x +
            (destination->work_area.width - mapped.width) / 2;
        mapped.y = destination->work_area.y +
            (destination->work_area.height - mapped.height) / 2;
    }
    return mapped;
}

UmiStatus umi_desktop_monitor_interaction_move_window(
    UmiDesktopMonitorInteraction *interaction,
    const char *window_id,
    const char *monitor_id)
{
    UmiDesktopMonitorTopology *topology;
    const UmiDesktopWindow *window;
    const UmiDesktopMonitor *source;
    const UmiDesktopMonitor *destination;
    UmiDesktopRect bounds;
    UmiStatus status;
    if (interaction == NULL || window_id == NULL || monitor_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    topology = umi_desktop_runtime_monitors(interaction->runtime);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(interaction->runtime), window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    source = umi_desktop_monitor_topology_find(topology, window->monitor_id);
    destination = umi_desktop_monitor_topology_find(topology, monitor_id);
    if (destination == NULL || !destination->enabled)
        return UMI_STATUS_NOT_FOUND;
    bounds = map_bounds(window->bounds, source, destination);
    status = umi_desktop_runtime_place_window(
        interaction->runtime, window_id, monitor_id, bounds,
        window->dock_placement);
    if (status == UMI_STATUS_OK) {
        window = umi_desktop_window_manager_find(
            umi_desktop_runtime_windows(interaction->runtime), window_id);
        status = remember_window(interaction, window);
    }
    if (status == UMI_STATUS_OK) {
        (void)copy_text(interaction->focused_monitor_id,
                        sizeof(interaction->focused_monitor_id), monitor_id);
        interaction->revision += 1U;
    }
    return status;
}

UmiStatus umi_desktop_monitor_interaction_move_window_next(
    UmiDesktopMonitorInteraction *interaction,
    const char *window_id)
{
    UmiDesktopMonitorTopology *topology;
    const UmiDesktopWindow *window;
    size_t current = SIZE_MAX;
    size_t offset;
    if (interaction == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    topology = umi_desktop_runtime_monitors(interaction->runtime);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(interaction->runtime), window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    for (offset = 0U; offset < topology->count; ++offset) {
        if (strcmp(topology->monitors[offset].monitor_id,
                   window->monitor_id) == 0) {
            current = offset;
            break;
        }
    }
    if (current == SIZE_MAX || enabled_monitor_count(topology) < 2U)
        return UMI_STATUS_INVALID_STATE;
    for (offset = 1U; offset <= topology->count; ++offset) {
        const size_t candidate = (current + offset) % topology->count;
        if (topology->monitors[candidate].enabled) {
            return umi_desktop_monitor_interaction_move_window(
                interaction, window_id,
                topology->monitors[candidate].monitor_id);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_desktop_monitor_interaction_restore_affinity(
    UmiDesktopMonitorInteraction *interaction,
    const char *window_id)
{
    const UmiDesktopMonitorAffinity *affinity;
    const UmiDesktopMonitor *monitor;
    const UmiDesktopWindow *window;
    size_t index;
    UmiStatus status;
    if (interaction == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_affinity(interaction, window_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    affinity = &interaction->affinities[index];
    monitor = umi_desktop_monitor_topology_find(
        umi_desktop_runtime_monitors(interaction->runtime),
        affinity->monitor_id);
    if (monitor == NULL || !monitor->enabled) return UMI_STATUS_UNAVAILABLE;
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(interaction->runtime), window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_desktop_runtime_restore_window_session(
        interaction->runtime, window_id, affinity->monitor_id,
        affinity->bounds, affinity->placement, window->visible,
        window->maximised);
    if (status == UMI_STATUS_OK) interaction->revision += 1U;
    return status;
}

UmiStatus umi_desktop_monitor_interaction_reconcile(
    UmiDesktopMonitorInteraction *interaction)
{
    UmiDesktopMonitorTopology *topology;
    UmiDesktopWindowManager *windows;
    const UmiDesktopMonitor *primary;
    size_t index;
    size_t relocated = 0U;
    if (interaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    topology = umi_desktop_runtime_monitors(interaction->runtime);
    windows = umi_desktop_runtime_windows(interaction->runtime);
    primary = umi_desktop_monitor_topology_primary(topology);
    if (primary == NULL || !primary->enabled)
        return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < windows->count; ++index) {
        UmiDesktopWindow *window = &windows->windows[index];
        const UmiDesktopMonitor *source = umi_desktop_monitor_topology_find(
            topology, window->monitor_id);
        UmiDesktopRect bounds;
        UmiStatus status;
        if (source != NULL && source->enabled) continue;
        bounds = map_bounds(window->bounds, source, primary);
        status = umi_desktop_runtime_restore_window_session(
            interaction->runtime, window->window_id, primary->monitor_id,
            bounds, window->dock_placement, window->visible,
            window->maximised);
        if (status != UMI_STATUS_OK) return status;
        relocated += 1U;
    }
    interaction->relocated_window_count += relocated;
    (void)copy_text(interaction->focused_monitor_id,
                    sizeof(interaction->focused_monitor_id),
                    primary->monitor_id);
    interaction->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_interaction_affinity_at(
    const UmiDesktopMonitorInteraction *interaction,
    size_t index,
    UmiDesktopMonitorAffinity *out_affinity)
{
    if (interaction == NULL || out_affinity == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= interaction->affinity_count) return UMI_STATUS_NOT_FOUND;
    *out_affinity = interaction->affinities[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_interaction_snapshot(
    const UmiDesktopMonitorInteraction *interaction,
    UmiDesktopMonitorInteractionSnapshot *out_snapshot)
{
    if (interaction == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    (void)snprintf(out_snapshot->focused_monitor_id,
                   sizeof(out_snapshot->focused_monitor_id), "%s",
                   interaction->focused_monitor_id);
    out_snapshot->enabled_monitor_count = enabled_monitor_count(
        umi_desktop_runtime_monitors(interaction->runtime));
    out_snapshot->affinity_count = interaction->affinity_count;
    out_snapshot->relocated_window_count =
        interaction->relocated_window_count;
    out_snapshot->revision = interaction->revision;
    return UMI_STATUS_OK;
}
