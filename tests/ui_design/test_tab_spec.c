/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_tab_spec.c
 *
 * PURPOSE:
 *   Verify the semantic tab spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/tab_spec.h"

int main(void){UmiDesignTabSpec s;if(umi_design_tab_spec_init(&s,1,1,1,0,UMI_DESIGN_ROLE_ACCENT)!=UMI_STATUS_OK)return 1;return s.dirty?0:2;}
