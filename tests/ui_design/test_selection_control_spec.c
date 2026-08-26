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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/selection_control_spec.h"

int main(void){UmiDesignSelectionControlSpec s;if(umi_design_selection_control_spec_init(&s,UMI_UI_COMPONENT_CHECK_BUTTON,1,1,0)!=UMI_STATUS_OK)return 1;return s.selected?0:2;}
