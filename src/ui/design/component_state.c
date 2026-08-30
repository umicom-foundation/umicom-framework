/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_state.c
 *
 * PURPOSE:
 *   Represent composable component state flags used consistently by all frontend renderers.
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

#include "umicom/ui/design/component_state.h"

void umi_design_component_state_add(UmiDesignComponentState *state,UmiDesignComponentStateFlag flag){if(state!=NULL)state->flags|=(uint32_t)flag;}
int umi_design_component_state_has(const UmiDesignComponentState *state,UmiDesignComponentStateFlag flag){return state!=NULL&&(state->flags&(uint32_t)flag)!=0U?1:0;}
