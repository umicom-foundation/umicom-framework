/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_list_spec.c
 *
 * PURPOSE:
 *   Verify the semantic list spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/list_spec.h"

int main(void){UmiDesignListSpec s;if(umi_design_list_spec_init(&s,50000U,UMI_DESIGN_DENSITY_COMPACT,1,1)!=UMI_STATUS_OK)return 1;return s.virtualised?0:2;}
