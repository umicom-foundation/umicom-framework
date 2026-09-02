/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/font_role.h
 *
 * PURPOSE:
 *   Define semantic font roles for product-neutral typography selection.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_FONT_ROLE_H
#define INCLUDE_UMICOM_UI_DESIGN_FONT_ROLE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named design font role values accepted by this public contract.
 */
typedef enum UmiDesignFontRole {
    UMI_DESIGN_FONT_BODY = 1,
    UMI_DESIGN_FONT_CAPTION = 2,
    UMI_DESIGN_FONT_LABEL = 3,
    UMI_DESIGN_FONT_TITLE = 4,
    UMI_DESIGN_FONT_DISPLAY = 5,
    UMI_DESIGN_FONT_MONO = 6
} UmiDesignFontRole;

/* Return a stable semantic name for the design enumeration value. */
const char *umi_design_font_role_name(UmiDesignFontRole value);

#ifdef __cplusplus
}
#endif

#endif
