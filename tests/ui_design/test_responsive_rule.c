/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_responsive_rule.c
 *
 * PURPOSE:
 *   Verify responsive rules retain density, columns and width matching.
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

int main(void){UmiDesignBreakpoint b;UmiDesignResponsiveRule r;if(umi_design_breakpoint_init(&b,"phone",0,599,UMI_DESIGN_SIZE_COMPACT)!=UMI_STATUS_OK)return 1;if(umi_design_responsive_rule_init(&r,&b,UMI_DESIGN_DENSITY_TOUCH,1U,1)!=UMI_STATUS_OK)return 2;return umi_design_responsive_rule_matches(&r,480)?0:3;}
