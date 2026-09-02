/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_navigation_spec.c
 *
 * PURPOSE:
 *   Verify the semantic navigation spec contract.
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

#include "umicom/ui/design/navigation_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignNavigationSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_navigation_spec_init(&s,UMI_UI_PLACEMENT_LEFT,7U,1,1)!=UMI_STATUS_OK)return 1;return s.item_count==7U?0:2;}
