/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_snap_engine.c
 *
 * PURPOSE:
 *   Implement the test snap engine behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop snap engine | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/snap_engine.h"

int main(void)
{
    UmiDesktopSnapPolicy policy;
    UmiDesktopMonitor monitor = {0};
    UmiDesktopWindowManager windows;
    UmiDesktopWindow peer = {0};
    UmiDesktopSnapResult result;
    (void)snprintf(monitor.monitor_id, sizeof(monitor.monitor_id), "primary");
    (void)snprintf(monitor.name, sizeof(monitor.name), "Primary");
    monitor.bounds = (UmiDesktopRect){0, 0, 1920, 1080};
    monitor.work_area = (UmiDesktopRect){0, 0, 1920, 1040};
    monitor.scale = 1.0;
    monitor.refresh_rate_hz = 60.0;
    monitor.enabled = true;
    (void)memset(&windows, 0, sizeof(windows));
    (void)snprintf(peer.window_id, sizeof(peer.window_id), "peer");
    (void)snprintf(peer.monitor_id, sizeof(peer.monitor_id), "primary");
    peer.bounds = (UmiDesktopRect){960, 0, 960, 1040};
    peer.visible = true;
    windows.windows[windows.count++] = peer;
    umi_desktop_snap_policy_init(&policy);
    assert(umi_desktop_snap_engine_apply(
        &policy, &monitor, &windows, "moving",
        (UmiDesktopRect){7, 12, 470, 311}, &result) == UMI_STATUS_OK);
    assert(result.bounds.x == 0);
    assert(result.bounds.y == 16);
    assert((result.guides & UMI_DESKTOP_SNAP_GUIDE_LEFT) != 0U);
    assert((result.guides & UMI_DESKTOP_SNAP_GUIDE_GRID_Y) != 0U);
    assert(umi_desktop_snap_engine_apply(
        &policy, &monitor, &windows, "moving",
        (UmiDesktopRect){483, 200, 470, 300}, &result) == UMI_STATUS_OK);
    assert(result.bounds.x == 490);
    assert((result.guides & UMI_DESKTOP_SNAP_GUIDE_PEER_X) != 0U);
    assert(umi_desktop_snap_engine_apply(
        &policy, &monitor, &windows, "moving",
        (UmiDesktopRect){400, 300, 20, 20}, &result) == UMI_STATUS_OK);
    assert(result.bounds.width == policy.minimum_width);
    assert(result.bounds.height == policy.minimum_height);
    return 0;
}
