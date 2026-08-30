/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_catalogue.c
 *
 * PURPOSE:
 *   Verify reusable semantic component registration and lookup.
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

#include "umicom/ui/design/component_catalogue.h"

int main(void){UmiDesignComponentCatalogue c={0};UmiDesignComponentDescriptor d,o;if(umi_design_component_descriptor_init(&d,"card","Card",UMI_UI_COMPONENT_FRAME,UMI_DESIGN_ROLE_NEUTRAL,0)!=UMI_STATUS_OK)return 1;if(umi_design_component_catalogue_upsert(&c,&d)!=UMI_STATUS_OK)return 2;return umi_design_component_catalogue_find(&c,"card",&o)==UMI_STATUS_OK&&c.revision==1U?0:3;}
