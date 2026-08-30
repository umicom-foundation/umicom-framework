/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_property_binding.c
 *
 * PURPOSE:
 *   Validate describe a visual property binding backed by the canonical reactive UI state layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/property_binding.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadPropertyBinding item;CHECK(umi_rad_property_binding_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_binding_is_valid(&item));return 0;}
