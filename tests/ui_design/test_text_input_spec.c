/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_text_input_spec.c
 *
 * PURPOSE:
 *   Verify the semantic text input spec contract.
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

#include "umicom/ui/design/text_input_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignTextInputSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_text_input_spec_init(&s,"Search…",256U,0,1,0)!=UMI_STATUS_OK)return 1;return s.search?0:2;}
