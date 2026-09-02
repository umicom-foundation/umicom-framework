/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_constrain_disabled.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology constrain disabled behavior.
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
 * Exercise test desktop monitor constrain disabled and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_constrain_disabled(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor monitor =
        umi_test_desktop_monitor(
            "off",
            "Off",
            0,
            0,
            100,
            100,
            false,
            false);
    UmiDesktopRect rectangle =
        {0, 0, 10, 10};

    umi_desktop_monitor_topology_init(&topology);

    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_topology_constrain(
        &topology,
        "off",
        &rectangle) == UMI_STATUS_NOT_FOUND);

    return 0;
}
