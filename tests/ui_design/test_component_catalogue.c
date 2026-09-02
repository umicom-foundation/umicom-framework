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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignComponentCatalogue c={0};UmiDesignComponentDescriptor d,o;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_descriptor_init(&d,"card","Card",UMI_UI_COMPONENT_FRAME,UMI_DESIGN_ROLE_NEUTRAL,0)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_catalogue_upsert(&c,&d)!=UMI_STATUS_OK)return 2;return umi_design_component_catalogue_find(&c,"card",&o)==UMI_STATUS_OK&&c.revision==1U?0:3;}
