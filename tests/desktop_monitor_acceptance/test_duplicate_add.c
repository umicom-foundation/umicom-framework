/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_duplicate_add.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology duplicate add behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "test_support.h"

int test_desktop_monitor_duplicate_add(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor monitor =
        umi_test_desktop_monitor(
            "one",
            "One",
            0,
            0,
            800,
            600,
            false,
            true);

    umi_desktop_monitor_topology_init(&topology);

    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_ALREADY_EXISTS);

    return 0;
}
