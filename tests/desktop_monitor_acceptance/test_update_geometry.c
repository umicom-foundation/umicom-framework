/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_update_geometry.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology update geometry behavior.
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
 * Exercise test desktop monitor update geometry and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_update_geometry(void)
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
    const UmiDesktopMonitor *found;

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);

    monitor.bounds.width = 1024;
    monitor.work_area.width = 1024;

    assert(umi_desktop_monitor_topology_update(
        &topology,
        &monitor) == UMI_STATUS_OK);

    found = umi_desktop_monitor_topology_find(
        &topology,
        "one");
    assert(found != NULL);
    assert(found->bounds.width == 1024);

    return 0;
}
