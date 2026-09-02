/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/theme_role.c
 *
 * PURPOSE:
 *   Implement semantic theme-role tokens used by GTK4 CSS and future frontend adapters instead of hard-coded product colours.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/theme_role.h"

/*
 * Provide the ws theme role token operation used by this module and its client
 * applications.
 */
const char *umi_ws_theme_role_token(UmiWsThemeRole role) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (role) {
        case UMI_WS_THEME_BACKGROUND: return "workstation.background";
        case UMI_WS_THEME_SURFACE: return "workstation.surface";
        case UMI_WS_THEME_SURFACE_ELEVATED: return "workstation.surface.elevated";
        case UMI_WS_THEME_BORDER: return "workstation.border";
        case UMI_WS_THEME_TEXT: return "workstation.text";
        case UMI_WS_THEME_TEXT_MUTED: return "workstation.text.muted";
        case UMI_WS_THEME_ACCENT: return "workstation.accent";
        case UMI_WS_THEME_SUCCESS: return "workstation.success";
        case UMI_WS_THEME_WARNING: return "workstation.warning";
        case UMI_WS_THEME_ERROR: return "workstation.error";
        case UMI_WS_THEME_FOCUS: return "workstation.focus";
        case UMI_WS_THEME_DRAG_TARGET: return "workstation.drag-target";
        default: return "workstation.surface";
    }
}

/*
 * Provide the ws theme role css class operation used by this module and its client
 * applications.
 */
const char *umi_ws_theme_role_css_class(UmiWsThemeRole role) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (role) {
        case UMI_WS_THEME_ACCENT: return "umi-accent";
        case UMI_WS_THEME_SUCCESS: return "umi-success";
        case UMI_WS_THEME_WARNING: return "umi-warning";
        case UMI_WS_THEME_ERROR: return "umi-error";
        case UMI_WS_THEME_FOCUS: return "umi-focus";
        case UMI_WS_THEME_DRAG_TARGET: return "umi-drag-target";
        case UMI_WS_THEME_TEXT_MUTED: return "umi-muted";
        case UMI_WS_THEME_BACKGROUND: return "umi-workstation-background";
        case UMI_WS_THEME_SURFACE_ELEVATED: return "umi-surface-elevated";
        case UMI_WS_THEME_BORDER: return "umi-border";
        case UMI_WS_THEME_TEXT: return "umi-text";
        case UMI_WS_THEME_SURFACE:
        default: return "umi-surface";
    }
}
