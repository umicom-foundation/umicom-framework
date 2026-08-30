/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_constrain_oversized.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology constrain oversized behavior.
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

int test_desktop_monitor_constrain_oversized(void)
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
        {0, 0, 1200, 900};

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_constrain(
        &topology,
        "screen",
        &rectangle) == UMI_STATUS_OK);
    assert(rectangle.width == 800);
    assert(rectangle.height == 600);

    return 0;
}
