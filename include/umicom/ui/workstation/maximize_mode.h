/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/maximize_mode.h
 *
 * PURPOSE:
 *   Define reversible panel maximisation state independent of persistent layout geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_MAXIMIZE_MODE_H
#define UMICOM_UI_WORKSTATION_MAXIMIZE_MODE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsMaximizeMode {
    char surface_id[UMI_UI_ID_CAPACITY];
    bool active;
} UmiWsMaximizeMode;

UmiStatus umi_ws_maximize_mode_activate(UmiWsMaximizeMode *mode, const char *surface_id);
void umi_ws_maximize_mode_restore(UmiWsMaximizeMode *mode);
bool umi_ws_maximize_mode_matches(const UmiWsMaximizeMode *mode, const char *surface_id);

#ifdef __cplusplus
}
#endif

#endif
