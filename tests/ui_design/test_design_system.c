/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_design_system.c
 *
 * PURPOSE:
 *   Verify aggregate design-system identity, defaults and revision tracking.
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

#include "umicom/ui/design/design_system.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignSystem s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_system_init(&s,"umicom",UMI_DESIGN_THEME_DARK,UMI_DESIGN_DENSITY_STANDARD)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_design_system_ready(&s))return 2;umi_design_system_touch(&s);return s.revision==1U?0:3;}
