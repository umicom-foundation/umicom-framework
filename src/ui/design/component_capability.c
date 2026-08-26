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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_capability.h"

void umi_design_component_capability_add(UmiDesignComponentCapability *capability,UmiDesignComponentCapabilityFlag flag){if(capability!=NULL)capability->flags|=(uint32_t)flag;}
int umi_design_component_capability_has(const UmiDesignComponentCapability *capability,UmiDesignComponentCapabilityFlag flag){return capability!=NULL&&(capability->flags&(uint32_t)flag)!=0U?1:0;}
