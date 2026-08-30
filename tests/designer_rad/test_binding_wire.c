/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_binding_wire.c
 *
 * PURPOSE:
 *   Validate represent a directed visual binding wire between endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/binding_wire.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadBindingWire item;CHECK(umi_rad_binding_wire_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_binding_wire_is_valid(&item));return 0;}
