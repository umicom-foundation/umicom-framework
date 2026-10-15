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

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include "value_archive/construction_cases.h"
#include "value_archive/construction_cases.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted page value. */
static int UmiFrontendPageConstructionCases(void)
{
    UmiFrontendPage *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, "home", "Home", "shell") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, oversized, "Home", "shell") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, value->id, "Home", "shell") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "home") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, "home", oversized, "shell") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, "home", value->title, "shell") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Home") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->component_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, "home", "Home", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_page_init(value, "home", "Home", value->component_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->component_id, "shell") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}


/* A rejected replacement must preserve the previously accepted route value. */
static int UmiFrontendRouteConstructionCases(void)
{
    UmiFrontendRoute *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_frontend_route_init(value, "/", "home") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_route_init(value, oversized, "home") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_route_init(value, value->path, "home") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "/") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->page_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_route_init(value, "/", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_frontend_route_init(value, "/", value->page_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->page_id, "home") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiFrontendRouteConstructionCases() != 0) return 1;

    if (UmiFrontendPageConstructionCases() != 0) return 1;
UmiFrontendPlan p;UmiFrontendPage page;UmiFrontendRoute route;char json[512];assert(umi_frontend_plan_init(&p,"org.umicom.demo",UMI_FRONTEND_KIND_WEB)==UMI_STATUS_OK);assert(umi_frontend_page_init(&page,"home","Home","shell")==UMI_STATUS_OK);assert(umi_frontend_route_init(&route,"/","home")==UMI_STATUS_OK);assert(umi_frontend_plan_add_page(&p,&page)==UMI_STATUS_OK);assert(umi_frontend_plan_add_route(&p,&route)==UMI_STATUS_OK);assert(umi_frontend_generate_manifest(&p,json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"org.umicom.demo")!=NULL);return 0;}
