/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/color_role.c
 *
 * PURPOSE:
 *   Define semantic colour roles independent of product palettes and frontend toolkits.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/color_role.h"

/*
 * Provide the design color role name operation used by this module and its client
 * applications.
 */
const char *umi_design_color_role_name(UmiDesignColorRole value)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (value) { case UMI_DESIGN_COLOR_BACKGROUND: return "background"; case UMI_DESIGN_COLOR_SURFACE: return "surface"; case UMI_DESIGN_COLOR_FOREGROUND: return "foreground"; case UMI_DESIGN_COLOR_MUTED: return "muted"; case UMI_DESIGN_COLOR_ACCENT: return "accent"; case UMI_DESIGN_COLOR_SUCCESS: return "success"; case UMI_DESIGN_COLOR_WARNING: return "warning"; case UMI_DESIGN_COLOR_DANGER: return "danger"; case UMI_DESIGN_COLOR_INFO: return "info"; case UMI_DESIGN_COLOR_BORDER: return "border"; default: return "unknown"; }
}
