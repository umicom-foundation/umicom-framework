/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/focus_ring.c
 *
 * PURPOSE:
 *   Define a consistent accessible focus indicator contract across all frontends.
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

#include "umicom/ui/design/focus_ring.h"

/*
 * Initialise design focus ring from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_focus_ring_init(UmiDesignFocusRing *ring,double width,double offset,UmiDesignColorRole color_role,double opacity){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ring==NULL||!umi_design_number_valid(width)||!umi_design_number_valid(offset)||!umi_design_number_valid(opacity)||width<=0.0||offset<0.0||opacity<=0.0||opacity>1.0||color_role<UMI_DESIGN_COLOR_BACKGROUND||color_role>UMI_DESIGN_COLOR_BORDER)return UMI_STATUS_INVALID_ARGUMENT;ring->width=width;ring->offset=offset;ring->color_role=color_role;ring->opacity=opacity;return UMI_STATUS_OK;}
