/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_remove_primary.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology remove primary behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_support.h"

int test_desktop_monitor_remove_primary(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor first =
        umi_test_desktop_monitor(
            "a", "A", 0, 0, 800, 600, true, true);
    UmiDesktopMonitor second =
        umi_test_desktop_monitor(
            "b", "B", 800, 0, 800, 600, false, true);
    const UmiDesktopMonitor *primary;

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &first) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &second) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_remove(
        &topology,
        "a") == UMI_STATUS_OK);

    primary = umi_desktop_monitor_topology_primary(&topology);
    assert(topology.count == 1U);
    assert(primary != NULL);
    assert(strcmp(primary->monitor_id, "b") == 0);

    return 0;
}
