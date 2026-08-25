/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/floating_window.h
 *
 * PURPOSE:
 *   Define toolkit-neutral floating-window placement used for secondary monitors and detached workstation panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_FLOATING_WINDOW_H
#define UMICOM_UI_WORKSTATION_FLOATING_WINDOW_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsFloatingWindow {
    char window_id[UMI_UI_ID_CAPACITY];
    char surface_id[UMI_UI_ID_CAPACITY];
    char monitor_id[UMI_UI_ID_CAPACITY];
    UmiUiRect bounds;
    bool always_on_top;
    bool maximised;
} UmiWsFloatingWindow;

UmiStatus umi_ws_floating_window_init(UmiWsFloatingWindow *window,
                                      const char *window_id,
                                      const char *surface_id,
                                      UmiUiRect bounds);
UmiStatus umi_ws_floating_window_assign_monitor(UmiWsFloatingWindow *window, const char *monitor_id);
void umi_ws_floating_window_set_maximised(UmiWsFloatingWindow *window, bool maximised);

#ifdef __cplusplus
}
#endif

#endif
