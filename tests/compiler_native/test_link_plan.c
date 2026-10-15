/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_link_plan.c
 *
 * PURPOSE:
 *   Regression coverage for build deterministic native link plans from objects, libraries, search roots and output kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/link_plan.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted link plan value. */
static int UmiNativeLinkPlanConstructionCases(void)
{
    UmiNativeLinkPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_link_plan_init(value, "app", UMI_NC_LINK_EXECUTABLE) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->output) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_link_plan_init(value, oversized, UMI_NC_LINK_EXECUTABLE) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_link_plan_init(value, value->output, UMI_NC_LINK_EXECUTABLE) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->output, "app") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeLinkPlanConstructionCases() != 0) return 1;
UmiNativeLinkPlan p;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_link_plan_init(&p,"app",UMI_NC_LINK_EXECUTABLE)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_link_plan_add_input(&p,"main.o")!=UMI_STATUS_OK||umi_nc_link_plan_add_library(&p,"m")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_link_plan_fingerprint(&p)==0U)return 3;return 0;}
