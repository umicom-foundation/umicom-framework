/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/state_style.h
 *
 * PURPOSE:
 *   Map an interaction state to semantic foreground, background, border and emphasis roles.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_STATE_STYLE_H
#define INCLUDE_UMICOM_UI_DESIGN_STATE_STYLE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/interaction_state.h"
#include "umicom/ui/design/color_role.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/interaction_state.h"
#include "umicom/ui/design/color_role.h"
/**
 * Represent the design state style data shared with callers of this public contract.
 */
typedef struct UmiDesignStateStyle { UmiDesignInteractionState state; UmiDesignColorRole foreground; UmiDesignColorRole background; UmiDesignColorRole border; double opacity; } UmiDesignStateStyle;
/* Initialise a validated state style with normalized opacity. */
UmiStatus umi_design_state_style_init(UmiDesignStateStyle *style, UmiDesignInteractionState state, UmiDesignColorRole foreground, UmiDesignColorRole background, UmiDesignColorRole border, double opacity);

#ifdef __cplusplus
}
#endif

#endif
