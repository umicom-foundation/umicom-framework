/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/focus_ring.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_FOCUS_RING_H
#define INCLUDE_UMICOM_UI_DESIGN_FOCUS_RING_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/color_role.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/color_role.h"
typedef struct UmiDesignFocusRing { double width; double offset; UmiDesignColorRole color_role; double opacity; } UmiDesignFocusRing;
/* Initialise an accessible focus-ring geometry and semantic colour role. */
UmiStatus umi_design_focus_ring_init(UmiDesignFocusRing *ring, double width, double offset, UmiDesignColorRole color_role, double opacity);

#ifdef __cplusplus
}
#endif

#endif
