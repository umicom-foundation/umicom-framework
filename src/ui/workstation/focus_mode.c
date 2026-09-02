/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/focus_mode.c
 *
 * PURPOSE:
 *   Implement reversible focus-mode state that can hide peripheral surfaces without mutating saved layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/focus_mode.h"

/*
 * Provide the ws focus mode enter operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_focus_mode_enter(UmiWsFocusMode *mode, const char *primary_surface_id, size_t hidden_surface_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mode == NULL || !umi_ws_id_valid(primary_surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *mode = (UmiWsFocusMode){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(mode->primary_surface_id, sizeof(mode->primary_surface_id), primary_surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    mode->hidden_surface_count = hidden_surface_count;
    mode->active = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws focus mode exit operation used by this module and its client
 * applications.
 */
void umi_ws_focus_mode_exit(UmiWsFocusMode *mode) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mode != NULL) {
        mode->active = false;
        mode->hidden_surface_count = 0U;
    }
}

/*
 * Provide the ws focus mode should hide operation used by this module and its client
 * applications.
 */
bool umi_ws_focus_mode_should_hide(const UmiWsFocusMode *mode, const char *surface_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mode == NULL || !mode->active || !umi_ws_id_valid(surface_id)) return false;
    return strcmp(mode->primary_surface_id, surface_id) != 0;
}
