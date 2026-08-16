/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/snap_engine.c
 *
 * PURPOSE:
 *   Implement Framework-authoritative layout snapping and visible guide flags.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/snap_engine.h"

#include <limits.h>
#include <string.h>

static int64_t distance_i32(int32_t first, int32_t second)
{
    int64_t distance = (int64_t)first - (int64_t)second;
    return distance < 0 ? -distance : distance;
}

static int32_t nearest_grid(int32_t value, int32_t grid)
{
    int32_t remainder;
    if (grid <= 1) return value;
    remainder = value % grid;
    if (remainder < 0) remainder += grid;
    if (remainder * 2 < grid) return value - remainder;
    return value + (grid - remainder);
}

static void snap_axis(
    int32_t *position,
    int32_t size,
    int32_t target_start,
    int32_t target_end,
    int32_t distance,
    uint32_t start_guide,
    uint32_t end_guide,
    uint32_t *guides)
{
    int64_t best = (int64_t)distance + 1;
    int32_t snapped = *position;
    uint32_t guide = 0U;
    int64_t value = distance_i32(*position, target_start);
    if (value <= distance && value < best) {
        best = value;
        snapped = target_start;
        guide = start_guide;
    }
    value = distance_i32(*position + size, target_end);
    if (value <= distance && value < best) {
        best = value;
        snapped = target_end - size;
        guide = end_guide;
    }
    value = distance_i32(*position, target_end);
    if (value <= distance && value < best) {
        best = value;
        snapped = target_end;
        guide = start_guide;
    }
    value = distance_i32(*position + size, target_start);
    if (value <= distance && value < best) {
        snapped = target_start - size;
        guide = end_guide;
    }
    if (guide != 0U) {
        *position = snapped;
        *guides |= guide;
    }
}

void umi_desktop_snap_policy_init(UmiDesktopSnapPolicy *policy)
{
    if (policy == NULL) return;
    policy->grid_size = 8;
    policy->snap_distance = 10;
    policy->minimum_width = 160;
    policy->minimum_height = 100;
    policy->enabled = true;
    policy->snap_to_grid = true;
    policy->snap_to_monitor = true;
    policy->snap_to_windows = true;
}

UmiStatus umi_desktop_snap_engine_apply(
    const UmiDesktopSnapPolicy *policy,
    const UmiDesktopMonitor *monitor,
    const UmiDesktopWindowManager *windows,
    const char *excluded_window_id,
    UmiDesktopRect candidate,
    UmiDesktopSnapResult *out_result)
{
    UmiDesktopRect original = candidate;
    size_t index;
    if (policy == NULL || monitor == NULL || out_result == NULL ||
        candidate.width <= 0 || candidate.height <= 0 ||
        policy->grid_size <= 0 || policy->snap_distance < 0 ||
        policy->minimum_width <= 0 || policy->minimum_height <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    if (candidate.width < policy->minimum_width)
        candidate.width = policy->minimum_width;
    if (candidate.height < policy->minimum_height)
        candidate.height = policy->minimum_height;
    if (policy->enabled && policy->snap_to_grid) {
        int32_t grid_x = nearest_grid(candidate.x, policy->grid_size);
        int32_t grid_y = nearest_grid(candidate.y, policy->grid_size);
        if (distance_i32(candidate.x, grid_x) <= policy->snap_distance) {
            candidate.x = grid_x;
            out_result->guides |= UMI_DESKTOP_SNAP_GUIDE_GRID_X;
        }
        if (distance_i32(candidate.y, grid_y) <= policy->snap_distance) {
            candidate.y = grid_y;
            out_result->guides |= UMI_DESKTOP_SNAP_GUIDE_GRID_Y;
        }
    }
    if (policy->enabled && policy->snap_to_monitor) {
        int32_t monitor_right = monitor->work_area.x + monitor->work_area.width;
        int32_t monitor_bottom = monitor->work_area.y + monitor->work_area.height;
        snap_axis(&candidate.x, candidate.width, monitor->work_area.x,
                  monitor_right, policy->snap_distance,
                  UMI_DESKTOP_SNAP_GUIDE_LEFT,
                  UMI_DESKTOP_SNAP_GUIDE_RIGHT, &out_result->guides);
        snap_axis(&candidate.y, candidate.height, monitor->work_area.y,
                  monitor_bottom, policy->snap_distance,
                  UMI_DESKTOP_SNAP_GUIDE_TOP,
                  UMI_DESKTOP_SNAP_GUIDE_BOTTOM, &out_result->guides);
        if (distance_i32(candidate.x + candidate.width / 2,
                         monitor->work_area.x + monitor->work_area.width / 2) <=
            policy->snap_distance) {
            candidate.x = monitor->work_area.x +
                (monitor->work_area.width - candidate.width) / 2;
            out_result->guides |= UMI_DESKTOP_SNAP_GUIDE_CENTRE_X;
        }
        if (distance_i32(candidate.y + candidate.height / 2,
                         monitor->work_area.y + monitor->work_area.height / 2) <=
            policy->snap_distance) {
            candidate.y = monitor->work_area.y +
                (monitor->work_area.height - candidate.height) / 2;
            out_result->guides |= UMI_DESKTOP_SNAP_GUIDE_CENTRE_Y;
        }
    }
    if (policy->enabled && policy->snap_to_windows && windows != NULL) {
        for (index = 0U; index < windows->count; ++index) {
            const UmiDesktopWindow *peer = &windows->windows[index];
            uint32_t before = out_result->guides;
            if (!peer->visible ||
                (excluded_window_id != NULL &&
                 strcmp(peer->window_id, excluded_window_id) == 0) ||
                strcmp(peer->monitor_id, monitor->monitor_id) != 0)
                continue;
            snap_axis(&candidate.x, candidate.width, peer->bounds.x,
                      peer->bounds.x + peer->bounds.width,
                      policy->snap_distance,
                      UMI_DESKTOP_SNAP_GUIDE_LEFT,
                      UMI_DESKTOP_SNAP_GUIDE_RIGHT, &out_result->guides);
            if (out_result->guides != before)
                out_result->guides |= UMI_DESKTOP_SNAP_GUIDE_PEER_X;
            before = out_result->guides;
            snap_axis(&candidate.y, candidate.height, peer->bounds.y,
                      peer->bounds.y + peer->bounds.height,
                      policy->snap_distance,
                      UMI_DESKTOP_SNAP_GUIDE_TOP,
                      UMI_DESKTOP_SNAP_GUIDE_BOTTOM, &out_result->guides);
            if (out_result->guides != before)
                out_result->guides |= UMI_DESKTOP_SNAP_GUIDE_PEER_Y;
        }
    }
    if (candidate.width > monitor->work_area.width)
        candidate.width = monitor->work_area.width;
    if (candidate.height > monitor->work_area.height)
        candidate.height = monitor->work_area.height;
    if (candidate.x < monitor->work_area.x) candidate.x = monitor->work_area.x;
    if (candidate.y < monitor->work_area.y) candidate.y = monitor->work_area.y;
    if ((int64_t)candidate.x + candidate.width >
        (int64_t)monitor->work_area.x + monitor->work_area.width)
        candidate.x = monitor->work_area.x + monitor->work_area.width -
            candidate.width;
    if ((int64_t)candidate.y + candidate.height >
        (int64_t)monitor->work_area.y + monitor->work_area.height)
        candidate.y = monitor->work_area.y + monitor->work_area.height -
            candidate.height;
    out_result->bounds = candidate;
    out_result->changed = memcmp(&candidate, &original, sizeof(candidate)) != 0;
    return UMI_STATUS_OK;
}
