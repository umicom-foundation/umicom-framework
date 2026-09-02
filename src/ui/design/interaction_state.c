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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/interaction_state.h"

/*
 * Provide the design interaction state name operation used by this module and its client
 * applications.
 */
const char *umi_design_interaction_state_name(UmiDesignInteractionState value)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (value) { case UMI_DESIGN_INTERACTION_REST: return "rest"; case UMI_DESIGN_INTERACTION_HOVER: return "hover"; case UMI_DESIGN_INTERACTION_ACTIVE: return "active"; case UMI_DESIGN_INTERACTION_FOCUS: return "focus"; case UMI_DESIGN_INTERACTION_DISABLED: return "disabled"; case UMI_DESIGN_INTERACTION_SELECTED: return "selected"; case UMI_DESIGN_INTERACTION_INVALID: return "invalid"; default: return "unknown"; }
}
