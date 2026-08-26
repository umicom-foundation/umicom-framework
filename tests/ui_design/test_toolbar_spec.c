/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_toolbar_spec.c
 *
 * PURPOSE:
 *   Verify the semantic toolbar spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/toolbar_spec.h"

int main(void){UmiDesignToolbarSpec s;if(umi_design_toolbar_spec_init(&s,UMI_UI_HORIZONTAL,UMI_DESIGN_DENSITY_COMPACT,8U,1)!=UMI_STATUS_OK)return 1;return s.overflow_menu?0:2;}
