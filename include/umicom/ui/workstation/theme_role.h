/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/theme_role.h
 *
 * PURPOSE:
 *   Define semantic theme-role tokens used by GTK4 CSS and future frontend adapters instead of hard-coded product colours.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_THEME_ROLE_H
#define UMICOM_UI_WORKSTATION_THEME_ROLE_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsThemeRole {
    UMI_WS_THEME_BACKGROUND = 1,
    UMI_WS_THEME_SURFACE = 2,
    UMI_WS_THEME_SURFACE_ELEVATED = 3,
    UMI_WS_THEME_BORDER = 4,
    UMI_WS_THEME_TEXT = 5,
    UMI_WS_THEME_TEXT_MUTED = 6,
    UMI_WS_THEME_ACCENT = 7,
    UMI_WS_THEME_SUCCESS = 8,
    UMI_WS_THEME_WARNING = 9,
    UMI_WS_THEME_ERROR = 10,
    UMI_WS_THEME_FOCUS = 11,
    UMI_WS_THEME_DRAG_TARGET = 12
} UmiWsThemeRole;

const char *umi_ws_theme_role_token(UmiWsThemeRole role);
const char *umi_ws_theme_role_css_class(UmiWsThemeRole role);

#ifdef __cplusplus
}
#endif

#endif
