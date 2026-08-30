/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_invalid_refresh.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology invalid refresh behavior.
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

int test_desktop_monitor_invalid_refresh(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor monitor =
        umi_test_desktop_monitor(
            "m",
            "M",
            0,
            0,
            100,
            100,
            false,
            true);

    monitor.refresh_rate_hz = 0.0;
    umi_desktop_monitor_topology_init(&topology);

    assert(umi_desktop_monitor_topology_add(
        &topology,
        &monitor) == UMI_STATUS_INVALID_ARGUMENT);

    return 0;
}
