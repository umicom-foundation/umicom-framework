/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_capability.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_CAPABILITY_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_CAPABILITY_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignComponentCapabilityFlag { UMI_DESIGN_CAP_FOCUS=1U<<0, UMI_DESIGN_CAP_KEYBOARD=1U<<1, UMI_DESIGN_CAP_POINTER=1U<<2, UMI_DESIGN_CAP_TOUCH=1U<<3, UMI_DESIGN_CAP_DRAG=1U<<4, UMI_DESIGN_CAP_DROP=1U<<5, UMI_DESIGN_CAP_SELECTION=1U<<6, UMI_DESIGN_CAP_EDIT=1U<<7 } UmiDesignComponentCapabilityFlag;
typedef struct UmiDesignComponentCapability { uint32_t flags; } UmiDesignComponentCapability;
/* Add one semantic interaction capability. */
void umi_design_component_capability_add(UmiDesignComponentCapability *capability, UmiDesignComponentCapabilityFlag flag);
/* Return one when a semantic interaction capability is supported. */
int umi_design_component_capability_has(const UmiDesignComponentCapability *capability, UmiDesignComponentCapabilityFlag flag);

#ifdef __cplusplus
}
#endif

#endif
