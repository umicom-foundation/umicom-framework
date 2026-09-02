/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_binding_endpoint.c
 *
 * PURPOSE:
 *   Validate represent one source or destination property endpoint in the visual binding editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/binding_endpoint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadBindingEndpoint item;CHECK(umi_rad_binding_endpoint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_binding_endpoint_is_valid(&item));return 0;}
