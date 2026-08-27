/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_capacity.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology capacity behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "test_support.h"

int test_desktop_monitor_capacity(void)
{
    UmiDesktopMonitorTopology topology;
    size_t index;

    umi_desktop_monitor_topology_init(&topology);

    for (index = 0U;
         index < UMI_DESKTOP_MAX_MONITORS;
         ++index) {
        UmiDesktopMonitor monitor =
            umi_test_desktop_monitor(
                "monitor",
                "Monitor",
                0,
                0,
                100,
                100,
                false,
                true);

        (void)snprintf(
            monitor.monitor_id,
            sizeof(monitor.monitor_id),
            "m%zu",
            index);

        assert(umi_desktop_monitor_topology_add(
            &topology,
            &monitor) == UMI_STATUS_OK);
    }

    {
        UmiDesktopMonitor overflow =
            umi_test_desktop_monitor(
                "overflow",
                "Overflow",
                0,
                0,
                100,
                100,
                false,
                true);

        assert(umi_desktop_monitor_topology_add(
            &topology,
            &overflow) == UMI_STATUS_CAPACITY_EXCEEDED);
    }

    return 0;
}
