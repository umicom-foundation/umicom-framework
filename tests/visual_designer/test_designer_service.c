/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_designer_service.c
 *
 * PURPOSE:
 *   Validate aggregate visual designer readiness and active-session state for thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/designer_service.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDesignerService item;CHECK(umi_rad_designer_service_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_designer_service_is_valid(&item));return 0;}
