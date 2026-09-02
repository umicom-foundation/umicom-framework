/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_capability.c
 *
 * PURPOSE:
 *   Represent semantic interaction capabilities required by reusable components and renderers.
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

#include "umicom/ui/design/component_capability.h"

/*
 * Add design component capability only after its inputs and available capacity have been
 * checked.
 */
void umi_design_component_capability_add(UmiDesignComponentCapability *capability,UmiDesignComponentCapabilityFlag flag){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(capability!=NULL)capability->flags|=(uint32_t)flag;}
/*
 * Provide the design component capability has operation used by this module and its client
 * applications.
 */
int umi_design_component_capability_has(const UmiDesignComponentCapability *capability,UmiDesignComponentCapabilityFlag flag){return capability!=NULL&&(capability->flags&(uint32_t)flag)!=0U?1:0;}
