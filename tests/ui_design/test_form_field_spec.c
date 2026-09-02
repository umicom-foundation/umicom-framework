/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_form_field_spec.c
 *
 * PURPOSE:
 *   Verify the semantic form field spec contract.
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

#include "umicom/ui/design/form_field_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignFormFieldSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_form_field_spec_init(&s,"Account","Required",1,UMI_UI_SEVERITY_INFORMATION)!=UMI_STATUS_OK)return 1;return s.required?0:2;}
