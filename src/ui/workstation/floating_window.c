/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/floating_window.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral floating-window placement used for secondary monitors and detached workstation panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/floating_window.h"

UmiStatus umi_ws_floating_window_init(UmiWsFloatingWindow *window,
                                      const char *window_id,
                                      const char *surface_id,
                                      UmiUiRect bounds) {
    if (window == NULL || !umi_ws_id_valid(window_id) || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (bounds.width <= 0 || bounds.height <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    *window = (UmiWsFloatingWindow){0};
    if (umi_ws_copy_text(window->window_id, sizeof(window->window_id), window_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(window->surface_id, sizeof(window->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    window->bounds = bounds;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_floating_window_assign_monitor(UmiWsFloatingWindow *window, const char *monitor_id) {
    if (window == NULL || !umi_ws_id_valid(monitor_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(window->monitor_id, sizeof(window->monitor_id), monitor_id);
}

void umi_ws_floating_window_set_maximised(UmiWsFloatingWindow *window, bool maximised) {
    if (window != NULL) window->maximised = maximised;
}
