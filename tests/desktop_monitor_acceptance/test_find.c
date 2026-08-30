/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_find.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology find behavior.
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

int test_desktop_monitor_find(void)
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

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_find(
        &topology,
        "screen") != NULL);
    assert(umi_desktop_monitor_topology_find(
        &topology,
        "missing") == NULL);

    return 0;
}
