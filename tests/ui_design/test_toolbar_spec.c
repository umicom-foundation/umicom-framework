/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_toolbar_spec.c
 *
 * PURPOSE:
 *   Verify the semantic toolbar spec contract.
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

#include "umicom/ui/design/toolbar_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignToolbarSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_toolbar_spec_init(&s,UMI_UI_HORIZONTAL,UMI_DESIGN_DENSITY_COMPACT,8U,1)!=UMI_STATUS_OK)return 1;return s.overflow_menu?0:2;}
