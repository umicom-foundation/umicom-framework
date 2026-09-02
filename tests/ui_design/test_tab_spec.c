/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_tab_spec.c
 *
 * PURPOSE:
 *   Verify the semantic tab spec contract.
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

#include "umicom/ui/design/tab_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignTabSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_tab_spec_init(&s,1,1,1,0,UMI_DESIGN_ROLE_ACCENT)!=UMI_STATUS_OK)return 1;return s.dirty?0:2;}
