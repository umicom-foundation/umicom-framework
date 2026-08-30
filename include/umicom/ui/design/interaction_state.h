/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/interaction_state.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_INTERACTION_STATE_H
#define INCLUDE_UMICOM_UI_DESIGN_INTERACTION_STATE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignInteractionState {
    UMI_DESIGN_INTERACTION_REST = 1,
    UMI_DESIGN_INTERACTION_HOVER = 2,
    UMI_DESIGN_INTERACTION_ACTIVE = 3,
    UMI_DESIGN_INTERACTION_FOCUS = 4,
    UMI_DESIGN_INTERACTION_DISABLED = 5,
    UMI_DESIGN_INTERACTION_SELECTED = 6,
    UMI_DESIGN_INTERACTION_INVALID = 7
} UmiDesignInteractionState;

/* Return a stable semantic name for the design enumeration value. */
const char *umi_design_interaction_state_name(UmiDesignInteractionState value);

#ifdef __cplusplus
}
#endif

#endif
