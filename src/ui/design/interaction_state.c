/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/interaction_state.c
 *
 * PURPOSE:
 *   Define mutually exclusive interaction states used for component style resolution.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/interaction_state.h"

const char *umi_design_interaction_state_name(UmiDesignInteractionState value)
{
    switch (value) { case UMI_DESIGN_INTERACTION_REST: return "rest"; case UMI_DESIGN_INTERACTION_HOVER: return "hover"; case UMI_DESIGN_INTERACTION_ACTIVE: return "active"; case UMI_DESIGN_INTERACTION_FOCUS: return "focus"; case UMI_DESIGN_INTERACTION_DISABLED: return "disabled"; case UMI_DESIGN_INTERACTION_SELECTED: return "selected"; case UMI_DESIGN_INTERACTION_INVALID: return "invalid"; default: return "unknown"; }
}
