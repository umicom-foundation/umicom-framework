/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/responsive_rule.c
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

#include "umicom/ui/design/responsive_rule.h"

/*
 * Initialise design responsive rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_responsive_rule_init(UmiDesignResponsiveRule *rule,const UmiDesignBreakpoint *breakpoint,UmiDesignDensity density,uint16_t columns,int visible){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(rule==NULL||breakpoint==NULL||columns==0U||density<UMI_DESIGN_DENSITY_COMPACT||density>UMI_DESIGN_DENSITY_TOUCH)return UMI_STATUS_INVALID_ARGUMENT;rule->breakpoint=*breakpoint;rule->density=density;rule->columns=columns;rule->visible=visible?1:0;return UMI_STATUS_OK;}
/*
 * Provide the design responsive rule matches operation used by this module and its client
 * applications.
 */
int umi_design_responsive_rule_matches(const UmiDesignResponsiveRule *rule,int32_t width){return rule!=NULL&&umi_design_breakpoint_matches(&rule->breakpoint,width)?1:0;}
