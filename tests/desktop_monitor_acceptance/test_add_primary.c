/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_add_primary.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology add primary behavior.
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

int test_desktop_monitor_add_primary(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor monitor =
        umi_test_desktop_monitor(
            "primary",
            "Primary",
            0,
            0,
            1920,
            1080,
            true,
            true);

    umi_desktop_monitor_topology_init(&topology);

    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_OK);
    assert(topology.count == 1U);
    assert(umi_desktop_monitor_topology_primary(
        &topology) != NULL);

    return 0;
}
