/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_binding_endpoint.c
 *
 * PURPOSE:
 *   Validate represent one source or destination property endpoint in the visual binding editor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/binding_endpoint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadBindingEndpoint item;CHECK(umi_rad_binding_endpoint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_binding_endpoint_is_valid(&item));return 0;}
