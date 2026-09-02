/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/color_role.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_COLOR_ROLE_H
#define INCLUDE_UMICOM_UI_DESIGN_COLOR_ROLE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named design color role values accepted by this public contract.
 */
typedef enum UmiDesignColorRole {
    UMI_DESIGN_COLOR_BACKGROUND = 1,
    UMI_DESIGN_COLOR_SURFACE = 2,
    UMI_DESIGN_COLOR_FOREGROUND = 3,
    UMI_DESIGN_COLOR_MUTED = 4,
    UMI_DESIGN_COLOR_ACCENT = 5,
    UMI_DESIGN_COLOR_SUCCESS = 6,
    UMI_DESIGN_COLOR_WARNING = 7,
    UMI_DESIGN_COLOR_DANGER = 8,
    UMI_DESIGN_COLOR_INFO = 9,
    UMI_DESIGN_COLOR_BORDER = 10
} UmiDesignColorRole;

/* Return a stable semantic name for the design enumeration value. */
const char *umi_design_color_role_name(UmiDesignColorRole value);

#ifdef __cplusplus
}
#endif

#endif
