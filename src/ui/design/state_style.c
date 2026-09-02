/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/state_style.c
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

#include "umicom/ui/design/state_style.h"

/*
 * Initialise design state style from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_state_style_init(UmiDesignStateStyle *style,UmiDesignInteractionState state,UmiDesignColorRole foreground,UmiDesignColorRole background,UmiDesignColorRole border,double opacity){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(style==NULL||state<UMI_DESIGN_INTERACTION_REST||state>UMI_DESIGN_INTERACTION_INVALID||foreground<UMI_DESIGN_COLOR_BACKGROUND||foreground>UMI_DESIGN_COLOR_BORDER||background<UMI_DESIGN_COLOR_BACKGROUND||background>UMI_DESIGN_COLOR_BORDER||border<UMI_DESIGN_COLOR_BACKGROUND||border>UMI_DESIGN_COLOR_BORDER||!umi_design_number_valid(opacity)||opacity<0.0||opacity>1.0)return UMI_STATUS_INVALID_ARGUMENT;style->state=state;style->foreground=foreground;style->background=background;style->border=border;style->opacity=opacity;return UMI_STATUS_OK;}
