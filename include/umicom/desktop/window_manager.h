/*-----------------------------------------------------------------------------
 * Umicom Framework | Federated desktop window manager
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_WINDOW_MANAGER_H
#define UMICOM_DESKTOP_WINDOW_MANAGER_H

#include "umicom/application/component_catalogue.h"
#include "umicom/base/status.h"
#include "umicom/desktop/monitor_topology.h"

typedef struct UmiDesktopWindowManager {
    UmiDesktopWindow windows[UMI_DESKTOP_MAX_WINDOWS];
    size_t count;
    uint64_t revision;
} UmiDesktopWindowManager;

void umi_desktop_window_manager_init(UmiDesktopWindowManager *manager);
UmiStatus umi_desktop_window_manager_open(
    UmiDesktopWindowManager *manager,
    const UmiDesktopMonitorTopology *topology,
    const UmiDesktopWindow *window);
UmiStatus umi_desktop_window_manager_close(
    UmiDesktopWindowManager *manager,
    const char *window_id);
UmiStatus umi_desktop_window_manager_show(
    UmiDesktopWindowManager *manager,
    const char *window_id,
    bool visible);
UmiStatus umi_desktop_window_manager_place(
    UmiDesktopWindowManager *manager,
    const UmiDesktopMonitorTopology *topology,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement);
UmiStatus umi_desktop_window_manager_maximise(
    UmiDesktopWindowManager *manager,
    const char *window_id,
    bool maximised);
UmiDesktopWindow *umi_desktop_window_manager_find_mutable(
    UmiDesktopWindowManager *manager,
    const char *window_id);
const UmiDesktopWindow *umi_desktop_window_manager_find(
    const UmiDesktopWindowManager *manager,
    const char *window_id);

#endif
