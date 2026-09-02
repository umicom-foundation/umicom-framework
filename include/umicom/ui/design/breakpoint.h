/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/breakpoint.h
 *
 * PURPOSE:
 *   Represent named responsive viewport ranges without coupling application code to CSS media queries.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_BREAKPOINT_H
#define INCLUDE_UMICOM_UI_DESIGN_BREAKPOINT_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/size_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/size_class.h"
/**
 * Represent the design breakpoint data shared with callers of this public contract.
 */
typedef struct UmiDesignBreakpoint { char id[UMI_DESIGN_ID_CAPACITY]; int32_t min_width; int32_t max_width; UmiDesignSizeClass size_class; } UmiDesignBreakpoint;
/* Initialise a named inclusive width breakpoint. */
UmiStatus umi_design_breakpoint_init(UmiDesignBreakpoint *bp, const char *id, int32_t min_width, int32_t max_width, UmiDesignSizeClass size_class);
/* Return one when a viewport width falls inside the inclusive breakpoint range. */
int umi_design_breakpoint_matches(const UmiDesignBreakpoint *bp, int32_t width);

#ifdef __cplusplus
}
#endif

#endif
