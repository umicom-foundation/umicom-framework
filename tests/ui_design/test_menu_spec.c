/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_menu_spec.c
 *
 * PURPOSE:
 *   Verify the semantic menu spec contract.
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

#include "umicom/ui/design/menu_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignMenuSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_menu_spec_init(&s,12U,1,1,1)!=UMI_STATUS_OK)return 1;return s.accelerators?0:2;}
