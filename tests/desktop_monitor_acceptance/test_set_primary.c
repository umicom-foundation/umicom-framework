/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_set_primary.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology set primary behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_support.h"

int test_desktop_monitor_set_primary(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor first =
        umi_test_desktop_monitor(
            "a", "A", 0, 0, 800, 600, false, true);
    UmiDesktopMonitor second =
        umi_test_desktop_monitor(
            "b", "B", 800, 0, 800, 600, false, true);

    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &first) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_topology_add(
        &topology,
        &second) == UMI_STATUS_OK);

    assert(umi_desktop_monitor_topology_set_primary(
        &topology,
        "b") == UMI_STATUS_OK);
    assert(strcmp(
        umi_desktop_monitor_topology_primary(
            &topology)->monitor_id,
        "b") == 0);

    return 0;
}
