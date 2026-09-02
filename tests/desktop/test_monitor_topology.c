/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_monitor_topology.c
 *
 * PURPOSE:
 *   Implement the test monitor topology behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop monitor topology | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/monitor_topology.h"

/*
 * Exercise monitor and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiDesktopMonitor monitor(const char *id, int32_t x, bool primary)
{
    UmiDesktopMonitor value = {0};
    (void)snprintf(value.monitor_id, sizeof(value.monitor_id), "%s", id);
    (void)snprintf(value.name, sizeof(value.name), "%s Monitor", id);
    value.bounds = (UmiDesktopRect){x, 0, 1920, 1080};
    value.work_area = (UmiDesktopRect){x, 0, 1920, 1040};
    value.scale = 1.0;
    value.refresh_rate_hz = 60.0;
    value.primary = primary;
    value.enabled = true;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopMonitorTopology topology;
    UmiDesktopMonitor first = monitor("first", 0, true);
    UmiDesktopMonitor second = monitor("second", 1920, false);
    UmiDesktopRect bounds = {3600, 900, 800, 600};
    umi_desktop_monitor_topology_init(&topology);
    assert(umi_desktop_monitor_topology_add(&topology, &first) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_topology_add(&topology, &second) == UMI_STATUS_OK);
    assert(topology.count == 2U);
    assert(strcmp(umi_desktop_monitor_topology_primary(&topology)->monitor_id,
                  "first") == 0);
    assert(umi_desktop_monitor_topology_set_primary(&topology, "second") ==
           UMI_STATUS_OK);
    assert(strcmp(umi_desktop_monitor_topology_primary(&topology)->monitor_id,
                  "second") == 0);
    assert(umi_desktop_monitor_topology_constrain(
        &topology, "second", &bounds) == UMI_STATUS_OK);
    assert(bounds.x == 3040 && bounds.y == 440);
    assert(umi_desktop_monitor_topology_remove(&topology, "second") ==
           UMI_STATUS_OK);
    assert(topology.count == 1U && topology.monitors[0].primary);
    return 0;
}
