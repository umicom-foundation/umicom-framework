/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_state.c
 *
 * PURPOSE:
 *   Verify composable component-state flags.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_state.h"

int main(void){UmiDesignComponentState s={0};umi_design_component_state_add(&s,UMI_DESIGN_STATE_FOCUSED);umi_design_component_state_add(&s,UMI_DESIGN_STATE_SELECTED);return umi_design_component_state_has(&s,UMI_DESIGN_STATE_SELECTED)?0:1;}
