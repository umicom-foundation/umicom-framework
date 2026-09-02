/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_property_editor_descriptor.c
 *
 * PURPOSE:
 *   Validate describe an editor choice for a semantic component property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/property_editor_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadPropertyEditorDescriptor item;CHECK(umi_rad_property_editor_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_editor_descriptor_is_valid(&item));return 0;}
