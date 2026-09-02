/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/monitor_topology.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral monitor topology, primary display selection and geometry matching for portable multi-monitor layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/monitor_topology.h"

#include <limits.h>
#include <string.h>

#include "internal.h"

/* Provide the rect area operation used by this module and its client applications. */
static int64_t rect_area(
    const UmiWorkbenchLayoutRect *rect)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rect == NULL || rect->width <= 0 || rect->height <= 0) {
        return 0;
    }
    return (int64_t)rect->width * (int64_t)rect->height;
}

/*
 * Provide the rect distance squared operation used by this module and its client
 * applications.
 */
static int64_t rect_distance_squared(
    const UmiWorkbenchLayoutRect *rect,
    const UmiWorkbenchLayoutRect *other)
{
    int64_t left_x;
    int64_t left_y;
    int64_t right_x;
    int64_t right_y;
    int64_t dx;
    int64_t dy;

    left_x = (int64_t)rect->x + (int64_t)rect->width / 2;
    left_y = (int64_t)rect->y + (int64_t)rect->height / 2;
    right_x = (int64_t)other->x + (int64_t)other->width / 2;
    right_y = (int64_t)other->y + (int64_t)other->height / 2;
    dx = left_x - right_x;
    dy = left_y - right_y;
    return dx * dx + dy * dy;
}

/*
 * Initialise workbench monitor topology from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_monitor_topology_init(
    UmiWorkbenchMonitorTopology *topology)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL) {
        return;
    }
    (void)memset(topology, 0, sizeof(*topology));
    topology->structure_size = sizeof(*topology);
    topology->revision = 1U;
}

/* Check that workbench monitor satisfies its contract before another service relies on it. */
UmiStatus umi_workbench_monitor_validate(
    const UmiWorkbenchMonitor *monitor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL ||
        monitor->structure_size < sizeof(*monitor) ||
        !umi_workbench_layout_text_present(monitor->monitor_id) ||
        !umi_workbench_layout_text_present(monitor->display_name) ||
        !umi_workbench_layout_rect_is_valid(&monitor->bounds) ||
        !umi_workbench_layout_rect_is_valid(&monitor->work_area) ||
        monitor->scale <= 0.0 ||
        monitor->refresh_rate_hz < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (monitor->work_area.x < monitor->bounds.x ||
        monitor->work_area.y < monitor->bounds.y ||
        (int64_t)monitor->work_area.x + monitor->work_area.width >
            (int64_t)monitor->bounds.x + monitor->bounds.width ||
        (int64_t)monitor->work_area.y + monitor->work_area.height >
            (int64_t)monitor->bounds.y + monitor->bounds.height) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Add workbench monitor topology only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_monitor_topology_add(
    UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchMonitor *monitor)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || monitor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_monitor_validate(monitor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (topology->count >= UMI_WORKBENCH_LAYOUT_MAX_MONITORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_monitor_topology_find(
            topology, monitor->monitor_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    topology->monitors[topology->count] = *monitor;
    topology->monitors[topology->count].structure_size =
        sizeof(topology->monitors[topology->count]);
    topology->count += 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (monitor->primary ||
        topology->primary_monitor_id[0] == '\0') {
        status = umi_workbench_monitor_topology_set_primary(
            topology, monitor->monitor_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            topology->count -= 1U;
            return status;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        topology->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Remove workbench monitor topology while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_monitor_topology_remove(
    UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    bool removed_primary;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL ||
        !umi_workbench_layout_text_present(monitor_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                topology->monitors[index].monitor_id,
                monitor_id) != 0) {
            continue;
        }

        removed_primary = topology->monitors[index].primary;
        /* Visit each bounded item once so every record receives the same rule. */
        for (; index + 1U < topology->count; ++index) {
            topology->monitors[index] =
                topology->monitors[index + 1U];
        }
        topology->count -= 1U;
        (void)memset(
            &topology->monitors[topology->count],
            0,
            sizeof(topology->monitors[topology->count]));
        topology->revision += 1U;

        /* Apply this branch only when its contract condition is satisfied. */
        if (removed_primary) {
            topology->primary_monitor_id[0] = '\0';
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (topology->count > 0U) {
                return umi_workbench_monitor_topology_set_primary(
                    topology,
                    topology->monitors[0].monitor_id);
            }
        }
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench monitor topology set primary operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_monitor_topology_set_primary(
    UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;
    bool found = false;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL ||
        !umi_workbench_layout_text_present(monitor_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        bool selected = strcmp(
            topology->monitors[index].monitor_id,
            monitor_id) == 0;
        topology->monitors[index].primary = selected;
        /* Apply this branch only when its contract condition is satisfied. */
        if (selected) {
            found = true;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!found) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_workbench_layout_copy_text(
        topology->primary_monitor_id,
        sizeof(topology->primary_monitor_id),
        monitor_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        topology->revision += 1U;
    }
    return status;
}

/*
 * Find workbench monitor topology while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_find(
    const UmiWorkbenchMonitorTopology *topology,
    const char *monitor_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL ||
        !umi_workbench_layout_text_present(monitor_id)) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                topology->monitors[index].monitor_id,
                monitor_id) == 0) {
            return &topology->monitors[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench monitor topology primary operation used by this module and its
 * client applications.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_primary(
    const UmiWorkbenchMonitorTopology *topology)
{
    return topology != NULL
        ? umi_workbench_monitor_topology_find(
              topology, topology->primary_monitor_id)
        : NULL;
}

/*
 * Find workbench monitor topology while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_at(
    const UmiWorkbenchMonitorTopology *topology,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || index >= topology->count) {
        return NULL;
    }
    return &topology->monitors[index];
}

/*
 * Provide the workbench monitor topology for point operation used by this module and its
 * client applications.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_for_point(
    const UmiWorkbenchMonitorTopology *topology,
    UmiWorkbenchLayoutPoint point)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (topology->monitors[index].enabled &&
            point.x >= topology->monitors[index].bounds.x &&
            point.y >= topology->monitors[index].bounds.y &&
            (int64_t)point.x <
                (int64_t)topology->monitors[index].bounds.x +
                    topology->monitors[index].bounds.width &&
            (int64_t)point.y <
                (int64_t)topology->monitors[index].bounds.y +
                    topology->monitors[index].bounds.height) {
            return &topology->monitors[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench monitor topology best for rect operation used by this module and
 * its client applications.
 */
const UmiWorkbenchMonitor *umi_workbench_monitor_topology_best_for_rect(
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutRect *rect)
{
    const UmiWorkbenchMonitor *best = NULL;
    int64_t best_overlap = -1;
    int64_t best_distance = INT64_MAX;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || rect == NULL) {
        return NULL;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        const UmiWorkbenchMonitor *monitor =
            &topology->monitors[index];
        UmiWorkbenchLayoutRect overlap;
        int64_t overlap_area;
        int64_t distance;

        /* Apply this operation only while the related capability or state is available. */
        if (!monitor->enabled) {
            continue;
        }
        overlap = umi_workbench_layout_rect_intersection(
            rect, &monitor->work_area);
        overlap_area = rect_area(&overlap);
        distance = rect_distance_squared(
            rect, &monitor->work_area);

        /* Apply this branch only when its contract condition is satisfied. */
        if (overlap_area > best_overlap ||
            (overlap_area == best_overlap &&
             distance < best_distance)) {
            best = monitor;
            best_overlap = overlap_area;
            best_distance = distance;
        }
    }

    return best != NULL
        ? best
        : umi_workbench_monitor_topology_primary(topology);
}
