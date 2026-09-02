/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/responsive_rule.h
 *
 * PURPOSE:
 *   Apply toolkit-neutral visibility, column and density decisions at responsive breakpoints.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_RESPONSIVE_RULE_H
#define INCLUDE_UMICOM_UI_DESIGN_RESPONSIVE_RULE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/breakpoint.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/breakpoint.h"
#include "umicom/ui/design/density.h"
/**
 * Represent the design responsive rule data shared with callers of this public contract.
 */
typedef struct UmiDesignResponsiveRule { UmiDesignBreakpoint breakpoint; UmiDesignDensity density; uint16_t columns; int visible; } UmiDesignResponsiveRule;
/* Initialise one responsive presentation rule. */
UmiStatus umi_design_responsive_rule_init(UmiDesignResponsiveRule *rule, const UmiDesignBreakpoint *breakpoint, UmiDesignDensity density, uint16_t columns, int visible);
/* Match a responsive rule to a width and expose its visibility decision. */
int umi_design_responsive_rule_matches(const UmiDesignResponsiveRule *rule, int32_t width);

#ifdef __cplusplus
}
#endif

#endif
