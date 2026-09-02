/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/window_manager.c
 *
 * PURPOSE:
 *   Implement the window manager behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Federated desktop window manager
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/window_manager.h"

#include <stdio.h>
#include <string.h>

/* Provide the find window index operation used by this module and its client applications. */
static size_t find_window_index(
    const UmiDesktopWindowManager *manager,
    const char *window_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || window_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manager->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(manager->windows[index].window_id, window_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

/* Provide the valid window operation used by this module and its client applications. */
static bool valid_window(const UmiDesktopWindow *window)
{
    return window != NULL && window->window_id[0] != '\0' &&
        window->title[0] != '\0' && window->component_id[0] != '\0' &&
        window->owner_application_id[0] != '\0' &&
        window->monitor_id[0] != '\0' && window->bounds.width > 0 &&
        window->bounds.height > 0 &&
        window->dock_placement >= UMI_DESKTOP_DOCK_CANVAS &&
        window->dock_placement <= UMI_DESKTOP_DOCK_FLOATING;
}

/*
 * Initialise desktop window manager from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_window_manager_init(UmiDesktopWindowManager *manager)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return;
    (void)memset(manager, 0, sizeof(*manager));
}

/*
 * Provide the desktop window manager open operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_open(
    UmiDesktopWindowManager *manager,
    const UmiDesktopMonitorTopology *topology,
    const UmiDesktopWindow *window)
{
    UmiDesktopWindow stored;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || topology == NULL || !valid_window(window))
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_component_catalogue_find(window->component_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_window_index(manager, window->window_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (manager->count >= UMI_DESKTOP_MAX_WINDOWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    stored = *window;
    status = umi_desktop_monitor_topology_constrain(
        topology, stored.monitor_id, &stored.bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    manager->windows[manager->count++] = stored;
    manager->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop window manager close operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_close(
    UmiDesktopWindowManager *manager,
    const char *window_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_window_index(manager, window_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!manager->windows[index].closable)
        return UMI_STATUS_PERMISSION_DENIED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < manager->count) {
        (void)memmove(&manager->windows[index], &manager->windows[index + 1U],
                      (manager->count - index - 1U) *
                          sizeof(manager->windows[0]));
    }
    manager->count -= 1U;
    (void)memset(&manager->windows[manager->count], 0,
                 sizeof(manager->windows[0]));
    manager->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop window manager show operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_show(
    UmiDesktopWindowManager *manager,
    const char *window_id,
    bool visible)
{
    UmiDesktopWindow *window =
        umi_desktop_window_manager_find_mutable(manager, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    window->visible = visible;
    manager->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop window manager place operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_place(
    UmiDesktopWindowManager *manager,
    const UmiDesktopMonitorTopology *topology,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement)
{
    UmiDesktopWindow *window;
    UmiStatus status;
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || topology == NULL || window_id == NULL ||
        monitor_id == NULL || monitor_id[0] == '\0' || bounds.width <= 0 ||
        bounds.height <= 0 || placement < UMI_DESKTOP_DOCK_CANVAS ||
        placement > UMI_DESKTOP_DOCK_FLOATING)
        return UMI_STATUS_INVALID_ARGUMENT;
    window = umi_desktop_window_manager_find_mutable(manager, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_desktop_monitor_topology_constrain(
        topology, monitor_id, &bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    length = snprintf(window->monitor_id, sizeof(window->monitor_id), "%s",
                      monitor_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(window->monitor_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    window->bounds = bounds;
    window->dock_placement = placement;
    window->maximised = false;
    manager->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop window manager maximise operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_manager_maximise(
    UmiDesktopWindowManager *manager,
    const char *window_id,
    bool maximised)
{
    UmiDesktopWindow *window =
        umi_desktop_window_manager_find_mutable(manager, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    window->maximised = maximised;
    manager->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop window manager find mutable operation used by this module and its
 * client applications.
 */
UmiDesktopWindow *umi_desktop_window_manager_find_mutable(
    UmiDesktopWindowManager *manager,
    const char *window_id)
{
    const size_t index = find_window_index(manager, window_id);
    return index != SIZE_MAX ? &manager->windows[index] : NULL;
}

/*
 * Find desktop window manager while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesktopWindow *umi_desktop_window_manager_find(
    const UmiDesktopWindowManager *manager,
    const char *window_id)
{
    const size_t index = find_window_index(manager, window_id);
    return index != SIZE_MAX ? &manager->windows[index] : NULL;
}
