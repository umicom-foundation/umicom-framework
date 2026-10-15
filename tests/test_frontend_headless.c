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
#include "value_archive/construction_cases.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted plan value. */
static int UmiFrontendPlanConstructionCases(void)
{
    UmiFrontendPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_frontend_plan_init(value, "org.umicom.demo", UMI_FRONTEND_KIND_HEADLESS) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->application_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_plan_init(value, oversized, UMI_FRONTEND_KIND_HEADLESS) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_plan_init(value, value->application_id, UMI_FRONTEND_KIND_HEADLESS) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->application_id, "org.umicom.demo") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiFrontendPlanConstructionCases() != 0) return 1;
UmiFrontendPlan p;UmiFrontendRenderer r;char capture[512];assert(umi_frontend_plan_init(&p,"org.umicom.demo",UMI_FRONTEND_KIND_HEADLESS)==UMI_STATUS_OK);assert(umi_frontend_headless_create(capture,sizeof(capture),&r)==UMI_STATUS_OK);assert(umi_frontend_renderer_validate(&r)==UMI_STATUS_OK);assert(r.render(r.instance,&p,".")==UMI_STATUS_OK);assert(strstr(capture,"org.umicom.demo")!=NULL);r.destroy(r.instance);return 0;}
