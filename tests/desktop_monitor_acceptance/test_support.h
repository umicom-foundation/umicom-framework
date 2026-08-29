/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_support.h
 *
 * PURPOSE:
 *   Provide test-only constructors for valid existing Framework desktop monitor records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_DESKTOP_MONITOR_ACCEPTANCE_SUPPORT_H
#define UMICOM_TEST_DESKTOP_MONITOR_ACCEPTANCE_SUPPORT_H

#include <string.h>

#include "umicom/desktop/monitor_topology.h"

static inline UmiDesktopMonitor umi_test_desktop_monitor(
    const char *monitor_id,
    const char *name,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height,
    bool primary,
    bool enabled)
{
    UmiDesktopMonitor monitor = {0};

    (void)strncpy(
        monitor.monitor_id,
        monitor_id,
        sizeof(monitor.monitor_id) - 1U);
    (void)strncpy(
        monitor.name,
        name,
        sizeof(monitor.name) - 1U);

    monitor.bounds =
        (UmiDesktopRect){x, y, width, height};
    monitor.work_area = monitor.bounds;
    monitor.scale = 1.0;
    monitor.refresh_rate_hz = 60.0;
    monitor.primary = primary;
    monitor.enabled = enabled;

    return monitor;
}

#endif
