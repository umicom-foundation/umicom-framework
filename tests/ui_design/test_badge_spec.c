/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_badge_spec.c
 *
 * PURPOSE:
 *   Verify the semantic badge spec contract.
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

#include "umicom/ui/design/badge_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignBadgeSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_badge_spec_init(&s,"Ready",UMI_DESIGN_ROLE_SUCCESS,0)!=UMI_STATUS_OK)return 1;return s.text[0]=='R'?0:2;}
