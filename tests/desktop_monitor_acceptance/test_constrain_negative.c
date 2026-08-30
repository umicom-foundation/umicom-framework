/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_constrain_negative.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology constrain negative behavior.
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

int test_desktop_monitor_constrain_negative(void)
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
        {-100, -50, 400, 300};

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_constrain(
        &topology,
        "screen",
        &rectangle) == UMI_STATUS_OK);
    assert(rectangle.x == 0);
    assert(rectangle.y == 0);

    return 0;
}
