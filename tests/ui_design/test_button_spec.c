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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/button_spec.h"

int main(void){UmiDesignButtonSpec s;if(umi_design_button_spec_init(&s,"Save",UMI_DESIGN_ROLE_PRIMARY,UMI_DESIGN_DENSITY_STANDARD,0,0)!=UMI_STATUS_OK)return 1;return s.label[0]=='S'?0:2;}
