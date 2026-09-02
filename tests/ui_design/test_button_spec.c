/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_button_spec.c
 *
 * PURPOSE:
 *   Verify the semantic button spec contract.
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

#include "umicom/ui/design/button_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignButtonSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_button_spec_init(&s,"Save",UMI_DESIGN_ROLE_PRIMARY,UMI_DESIGN_DENSITY_STANDARD,0,0)!=UMI_STATUS_OK)return 1;return s.label[0]=='S'?0:2;}
