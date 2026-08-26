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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/responsive_rule.h"

UmiStatus umi_design_responsive_rule_init(UmiDesignResponsiveRule *rule,const UmiDesignBreakpoint *breakpoint,UmiDesignDensity density,uint16_t columns,int visible){if(rule==NULL||breakpoint==NULL||columns==0U||density<UMI_DESIGN_DENSITY_COMPACT||density>UMI_DESIGN_DENSITY_TOUCH)return UMI_STATUS_INVALID_ARGUMENT;rule->breakpoint=*breakpoint;rule->density=density;rule->columns=columns;rule->visible=visible?1:0;return UMI_STATUS_OK;}
int umi_design_responsive_rule_matches(const UmiDesignResponsiveRule *rule,int32_t width){return rule!=NULL&&umi_design_breakpoint_matches(&rule->breakpoint,width)?1:0;}
