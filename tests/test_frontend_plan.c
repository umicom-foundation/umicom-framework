/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_plan.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
int main(void){UmiFrontendPlan p;UmiFrontendPage page;UmiFrontendRoute route;char json[512];assert(umi_frontend_plan_init(&p,"org.umicom.demo",UMI_FRONTEND_KIND_WEB)==UMI_STATUS_OK);assert(umi_frontend_page_init(&page,"home","Home","shell")==UMI_STATUS_OK);assert(umi_frontend_route_init(&route,"/","home")==UMI_STATUS_OK);assert(umi_frontend_plan_add_page(&p,&page)==UMI_STATUS_OK);assert(umi_frontend_plan_add_route(&p,&route)==UMI_STATUS_OK);assert(umi_frontend_generate_manifest(&p,json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"org.umicom.demo")!=NULL);return 0;}
