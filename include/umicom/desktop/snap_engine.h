/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/snap_engine.h
 *
 * PURPOSE:
 *   Provide deterministic grid, monitor-edge and peer-window snapping for the
 *   visual layout designer without embedding geometry policy in a frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_SNAP_ENGINE_H
#define UMICOM_DESKTOP_SNAP_ENGINE_H

#include "umicom/desktop/window_manager.h"

#define UMI_DESKTOP_SNAP_GUIDE_LEFT          (1U << 0U)
#define UMI_DESKTOP_SNAP_GUIDE_RIGHT         (1U << 1U)
#define UMI_DESKTOP_SNAP_GUIDE_TOP           (1U << 2U)
#define UMI_DESKTOP_SNAP_GUIDE_BOTTOM        (1U << 3U)
#define UMI_DESKTOP_SNAP_GUIDE_CENTRE_X      (1U << 4U)
#define UMI_DESKTOP_SNAP_GUIDE_CENTRE_Y      (1U << 5U)
#define UMI_DESKTOP_SNAP_GUIDE_GRID_X        (1U << 6U)
#define UMI_DESKTOP_SNAP_GUIDE_GRID_Y        (1U << 7U)
#define UMI_DESKTOP_SNAP_GUIDE_PEER_X        (1U << 8U)
#define UMI_DESKTOP_SNAP_GUIDE_PEER_Y        (1U << 9U)

typedef struct UmiDesktopSnapPolicy {
    int32_t grid_size;
    int32_t snap_distance;
    int32_t minimum_width;
    int32_t minimum_height;
    bool enabled;
    bool snap_to_grid;
    bool snap_to_monitor;
    bool snap_to_windows;
} UmiDesktopSnapPolicy;

typedef struct UmiDesktopSnapResult {
    UmiDesktopRect bounds;
    uint32_t guides;
    bool changed;
} UmiDesktopSnapResult;

void umi_desktop_snap_policy_init(UmiDesktopSnapPolicy *policy);
UmiStatus umi_desktop_snap_engine_apply(
    const UmiDesktopSnapPolicy *policy,
    const UmiDesktopMonitor *monitor,
    const UmiDesktopWindowManager *windows,
    const char *excluded_window_id,
    UmiDesktopRect candidate,
    UmiDesktopSnapResult *out_result);

#endif
