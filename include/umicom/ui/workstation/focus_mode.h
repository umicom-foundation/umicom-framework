/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/focus_mode.h
 *
 * PURPOSE:
 *   Define reversible focus-mode state that can hide peripheral surfaces without mutating saved layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_FOCUS_MODE_H
#define UMICOM_UI_WORKSTATION_FOCUS_MODE_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws focus mode data shared with callers of this public contract.
 */
typedef struct UmiWsFocusMode {
    char primary_surface_id[UMI_UI_ID_CAPACITY];
    size_t hidden_surface_count;
    bool active;
} UmiWsFocusMode;

/**
 * Provide the ws focus mode enter operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_focus_mode_enter(UmiWsFocusMode *mode, const char *primary_surface_id, size_t hidden_surface_count);
/**
 * Provide the ws focus mode exit operation used by this module and its client
 * applications.
 */
void umi_ws_focus_mode_exit(UmiWsFocusMode *mode);
/**
 * Provide the ws focus mode should hide operation used by this module and its client
 * applications.
 */
bool umi_ws_focus_mode_should_hide(const UmiWsFocusMode *mode, const char *surface_id);

#ifdef __cplusplus
}
#endif

#endif
