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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/responsive_rule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignBreakpoint b;UmiDesignResponsiveRule r;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_breakpoint_init(&b,"phone",0,599,UMI_DESIGN_SIZE_COMPACT)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_responsive_rule_init(&r,&b,UMI_DESIGN_DENSITY_TOUCH,1U,1)!=UMI_STATUS_OK)return 2;return umi_design_responsive_rule_matches(&r,480)?0:3;}
