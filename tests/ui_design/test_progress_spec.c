/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_progress_spec.c
 *
 * PURPOSE:
 *   Verify the semantic progress spec contract.
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

#include "umicom/ui/design/progress_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignProgressSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_progress_spec_init(&s,0.0,100.0,75.0,0,1)!=UMI_STATUS_OK)return 1;return s.value==75.0?0:2;}
