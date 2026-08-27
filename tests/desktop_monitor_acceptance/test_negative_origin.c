/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_negative_origin.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology negative origin behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "test_support.h"

int test_desktop_monitor_negative_origin(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor monitor =
        umi_test_desktop_monitor(
            "left",
            "Left",
            -1920,
            0,
            1920,
            1080,
            false,
            true);
    UmiDesktopRect rectangle =
        {-2500, 20, 500, 400};

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_constrain(
        &topology,
        "left",
        &rectangle) == UMI_STATUS_OK);
    assert(rectangle.x == -1920);

    return 0;
}
