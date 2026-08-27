/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_remove_missing.c
 *
 * PURPOSE:
 *   Verify the existing desktop monitor-topology remove missing behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "test_support.h"

int test_desktop_monitor_remove_missing(void)
{
    UmiDesktopMonitorTopology topology;

    umi_desktop_monitor_topology_init(&topology);

    assert(umi_desktop_monitor_topology_remove(
        &topology,
        "missing") == UMI_STATUS_NOT_FOUND);

    return 0;
}
