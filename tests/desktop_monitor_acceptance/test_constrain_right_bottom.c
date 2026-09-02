/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_constrain_right_bottom.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology constrain right bottom behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "test_support.h"

/*
 * Exercise test desktop monitor constrain right bottom and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_constrain_right_bottom(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor monitor =
        umi_test_desktop_monitor(
            "screen",
            "Screen",
            0,
            0,
            800,
            600,
            false,
            true);
    UmiDesktopRect rectangle =
        {700, 550, 300, 200};

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_constrain(
        &topology,
        "screen",
        &rectangle) == UMI_STATUS_OK);
    assert(rectangle.x == 500);
    assert(rectangle.y == 400);

    return 0;
}
