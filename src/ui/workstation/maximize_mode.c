/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/maximize_mode.c
 *
 * PURPOSE:
 *   Implement reversible panel maximisation state independent of persistent layout geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/maximize_mode.h"

/*
 * Provide the ws maximize mode activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_maximize_mode_activate(UmiWsMaximizeMode *mode, const char *surface_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mode == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *mode = (UmiWsMaximizeMode){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(mode->surface_id, sizeof(mode->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    mode->active = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws maximize mode restore operation used by this module and its client
 * applications.
 */
void umi_ws_maximize_mode_restore(UmiWsMaximizeMode *mode) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mode != NULL) mode->active = false;
}

/*
 * Provide the ws maximize mode matches operation used by this module and its client
 * applications.
 */
bool umi_ws_maximize_mode_matches(const UmiWsMaximizeMode *mode, const char *surface_id) {
    return mode != NULL && mode->active && umi_ws_id_valid(surface_id) && strcmp(mode->surface_id, surface_id) == 0;
}
