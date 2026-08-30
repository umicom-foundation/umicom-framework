/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_designer_service.c
 *
 * PURPOSE:
 *   Validate aggregate RAD designer readiness and active-session state for thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/designer_service.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDesignerService item;CHECK(umi_rad_designer_service_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_designer_service_is_valid(&item));return 0;}
