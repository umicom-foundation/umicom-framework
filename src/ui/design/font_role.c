/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/font_role.c
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

#include "umicom/ui/design/font_role.h"

/*
 * Provide the design font role name operation used by this module and its client
 * applications.
 */
const char *umi_design_font_role_name(UmiDesignFontRole value)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (value) { case UMI_DESIGN_FONT_BODY: return "body"; case UMI_DESIGN_FONT_CAPTION: return "caption"; case UMI_DESIGN_FONT_LABEL: return "label"; case UMI_DESIGN_FONT_TITLE: return "title"; case UMI_DESIGN_FONT_DISPLAY: return "display"; case UMI_DESIGN_FONT_MONO: return "mono"; default: return "unknown"; }
}
