/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_property_editor_descriptor.c
 *
 * PURPOSE:
 *   Validate describe an editor choice for a semantic component property.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/property_editor_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadPropertyEditorDescriptor item;CHECK(umi_rad_property_editor_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_editor_descriptor_is_valid(&item));return 0;}
