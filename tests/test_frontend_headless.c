/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_headless.c
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

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiFrontendPlan p;UmiFrontendRenderer r;char capture[512];assert(umi_frontend_plan_init(&p,"org.umicom.demo",UMI_FRONTEND_KIND_HEADLESS)==UMI_STATUS_OK);assert(umi_frontend_headless_create(capture,sizeof(capture),&r)==UMI_STATUS_OK);assert(umi_frontend_renderer_validate(&r)==UMI_STATUS_OK);assert(r.render(r.instance,&p,".")==UMI_STATUS_OK);assert(strstr(capture,"org.umicom.demo")!=NULL);r.destroy(r.instance);return 0;}
