/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/floating_window.h
 *
 * PURPOSE:
 *   Define toolkit-neutral floating-window placement used for secondary monitors and detached workstation panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_FLOATING_WINDOW_H
#define UMICOM_UI_WORKSTATION_FLOATING_WINDOW_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws floating window data shared with callers of this public contract.
 */
typedef struct UmiWsFloatingWindow {
    char window_id[UMI_UI_ID_CAPACITY];
    char surface_id[UMI_UI_ID_CAPACITY];
    char monitor_id[UMI_UI_ID_CAPACITY];
    UmiUiRect bounds;
    bool always_on_top;
    bool maximised;
} UmiWsFloatingWindow;

/**
 * Initialise ws floating window from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_floating_window_init(UmiWsFloatingWindow *window,
                                      const char *window_id,
                                      const char *surface_id,
                                      UmiUiRect bounds);
/**
 * Provide the ws floating window assign monitor operation used by this module and its
 * client applications.
 */
UmiStatus umi_ws_floating_window_assign_monitor(UmiWsFloatingWindow *window, const char *monitor_id);
/**
 * Provide the ws floating window set maximised operation used by this module and its
 * client applications.
 */
void umi_ws_floating_window_set_maximised(UmiWsFloatingWindow *window, bool maximised);

#ifdef __cplusplus
}
#endif

#endif
