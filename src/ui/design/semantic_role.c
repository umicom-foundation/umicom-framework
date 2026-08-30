/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/semantic_role.c
 *
 * PURPOSE:
 *   Define visual intent roles shared by controls, analytics surfaces and application brands.
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

#include "umicom/ui/design/semantic_role.h"

const char *umi_design_semantic_role_name(UmiDesignSemanticRole value)
{
    switch (value) { case UMI_DESIGN_ROLE_NEUTRAL: return "neutral"; case UMI_DESIGN_ROLE_PRIMARY: return "primary"; case UMI_DESIGN_ROLE_SECONDARY: return "secondary"; case UMI_DESIGN_ROLE_INFO: return "info"; case UMI_DESIGN_ROLE_SUCCESS: return "success"; case UMI_DESIGN_ROLE_WARNING: return "warning"; case UMI_DESIGN_ROLE_DANGER: return "danger"; case UMI_DESIGN_ROLE_ACCENT: return "accent"; default: return "unknown"; }
}
