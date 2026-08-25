/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/maximize_mode.c
 *
 * PURPOSE:
 *   Implement reversible panel maximisation state independent of persistent layout geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/maximize_mode.h"

UmiStatus umi_ws_maximize_mode_activate(UmiWsMaximizeMode *mode, const char *surface_id) {
    if (mode == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *mode = (UmiWsMaximizeMode){0};
    if (umi_ws_copy_text(mode->surface_id, sizeof(mode->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    mode->active = true;
    return UMI_STATUS_OK;
}

void umi_ws_maximize_mode_restore(UmiWsMaximizeMode *mode) {
    if (mode != NULL) mode->active = false;
}

bool umi_ws_maximize_mode_matches(const UmiWsMaximizeMode *mode, const char *surface_id) {
    return mode != NULL && mode->active && umi_ws_id_valid(surface_id) && strcmp(mode->surface_id, surface_id) == 0;
}
