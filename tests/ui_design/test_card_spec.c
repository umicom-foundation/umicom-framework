/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_card_spec.c
 *
 * PURPOSE:
 *   Verify the semantic card spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/card_spec.h"

int main(void){UmiDesignCardSpec s;if(umi_design_card_spec_init(&s,UMI_DESIGN_ROLE_NEUTRAL,2U,1,0)!=UMI_STATUS_OK)return 1;return s.elevation_level==2U?0:2;}
