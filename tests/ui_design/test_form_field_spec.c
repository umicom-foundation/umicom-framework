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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/form_field_spec.h"

int main(void){UmiDesignFormFieldSpec s;if(umi_design_form_field_spec_init(&s,"Account","Required",1,UMI_UI_SEVERITY_INFORMATION)!=UMI_STATUS_OK)return 1;return s.required?0:2;}
