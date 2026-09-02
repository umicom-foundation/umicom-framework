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

/*
 * Initialise ws floating window from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_floating_window_init(UmiWsFloatingWindow *window,
                                      const char *window_id,
                                      const char *surface_id,
                                      UmiUiRect bounds) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || !umi_ws_id_valid(window_id) || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (bounds.width <= 0 || bounds.height <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    *window = (UmiWsFloatingWindow){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(window->window_id, sizeof(window->window_id), window_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(window->surface_id, sizeof(window->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    window->bounds = bounds;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws floating window assign monitor operation used by this module and its
 * client applications.
 */
UmiStatus umi_ws_floating_window_assign_monitor(UmiWsFloatingWindow *window, const char *monitor_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || !umi_ws_id_valid(monitor_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(window->monitor_id, sizeof(window->monitor_id), monitor_id);
}

/*
 * Provide the ws floating window set maximised operation used by this module and its
 * client applications.
 */
void umi_ws_floating_window_set_maximised(UmiWsFloatingWindow *window, bool maximised) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window != NULL) window->maximised = maximised;
}
