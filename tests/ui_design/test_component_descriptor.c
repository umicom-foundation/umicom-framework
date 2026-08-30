/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_descriptor.c
 *
 * PURPOSE:
 *   Verify semantic descriptors augment canonical component kinds without replacing them.
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

#include "umicom/ui/design/component_descriptor.h"

int main(void){UmiDesignComponentDescriptor d;if(umi_design_component_descriptor_init(&d,"button.primary","Primary Button",UMI_UI_COMPONENT_BUTTON,UMI_DESIGN_ROLE_PRIMARY,1)!=UMI_STATUS_OK)return 1;return umi_design_component_descriptor_valid(&d)?0:2;}
