/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_alert_spec.c
 *
 * PURPOSE:
 *   Verify the semantic alert spec contract.
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

#include "umicom/ui/design/alert_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignAlertSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_alert_spec_init(&s,UMI_UI_SEVERITY_WARNING,"Connection degraded",1,1)!=UMI_STATUS_OK)return 1;return s.actionable?0:2;}
