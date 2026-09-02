/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/window_manager.h
 *
 * PURPOSE:
 *   Publish the public window manager contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_WINDOW_MANAGER_H
#define UMICOM_DESKTOP_WINDOW_MANAGER_H

#include "umicom/application/component_catalogue.h"
#include "umicom/base/status.h"
#include "umicom/desktop/monitor_topology.h"

/**
 * Represent the desktop window manager data shared with callers of this public contract.
 */
typedef struct UmiDesktopWindowManager {
    UmiDesktopWindow windows[UMI_DESKTOP_MAX_WINDOWS];
    size_t count;
    uint64_t revision;
} UmiDesktopWindowManager;

/**
 * Initialise desktop window manager from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_window_manager_init(UmiDesktopWindowManager *manager);
/**
 * Provide the desktop window manager open operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_open(
    UmiDesktopWindowManager *manager,
    const UmiDesktopMonitorTopology *topology,
    const UmiDesktopWindow *window);
/**
 * Provide the desktop window manager close operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_close(
    UmiDesktopWindowManager *manager,
    const char *window_id);
/**
 * Provide the desktop window manager show operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_show(
    UmiDesktopWindowManager *manager,
    const char *window_id,
    bool visible);
/**
 * Provide the desktop window manager place operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_place(
    UmiDesktopWindowManager *manager,
    const UmiDesktopMonitorTopology *topology,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement);
/**
 * Provide the desktop window manager maximise operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_maximise(
    UmiDesktopWindowManager *manager,
    const char *window_id,
    bool maximised);
/**
 * Provide the desktop window manager find mutable operation used by this module and its
 * client applications.
 */
UmiDesktopWindow *umi_desktop_window_manager_find_mutable(
    UmiDesktopWindowManager *manager,
    const char *window_id);
/**
 * Find desktop window manager while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesktopWindow *umi_desktop_window_manager_find(
    const UmiDesktopWindowManager *manager,
    const char *window_id);

#endif
