/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_menu_spec.c
 *
 * PURPOSE:
 *   Verify the semantic menu spec contract.
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

#include "umicom/ui/design/menu_spec.h"

int main(void){UmiDesignMenuSpec s;if(umi_design_menu_spec_init(&s,12U,1,1,1)!=UMI_STATUS_OK)return 1;return s.accelerators?0:2;}
