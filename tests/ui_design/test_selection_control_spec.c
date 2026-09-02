/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_selection_control_spec.c
 *
 * PURPOSE:
 *   Verify the semantic selection control spec contract.
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

#include "umicom/ui/design/selection_control_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignSelectionControlSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_selection_control_spec_init(&s,UMI_UI_COMPONENT_CHECK_BUTTON,1,1,0)!=UMI_STATUS_OK)return 1;return s.selected?0:2;}
