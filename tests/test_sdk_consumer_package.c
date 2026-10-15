/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sdk_consumer_package.c
 *
 * PURPOSE:
 *   Verify installed SDK export plans and generated consumer CMake text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "value_archive/construction_cases.h"
#include <string.h>
#include "umicom/sdk/sdk.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted export plan value. */
static int UmiSdkExportPlanConstructionCases(void)
{
    UmiSdkExportPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, "C:/Umicom/Framework", "0.9") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->install_prefix) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, oversized, "0.9") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, value->install_prefix, "0.9") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->install_prefix, "C:/Umicom/Framework") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiSdkExportPlanConstructionCases() != 0) return 1;

    UmiSdkExportPlan plan;
    char cmake[1024];
    assert(umi_sdk_export_plan_init(
               &plan, "C:/Umicom/Framework", "0.9") == UMI_STATUS_OK);
    assert(umi_sdk_export_plan_add_target(
               &plan, "sdk", "Umicom::sdk", 1) == UMI_STATUS_OK);
    assert(umi_sdk_export_plan_add_target(
               &plan, "delivery", "Umicom::delivery", 1) == UMI_STATUS_OK);
    assert(umi_sdk_export_plan_validate(&plan) == UMI_STATUS_OK);
    assert(umi_sdk_consumer_package_render_cmake(
               &plan.package, &plan.catalogue, cmake, sizeof(cmake)) ==
           UMI_STATUS_OK);
    assert(strstr(cmake, "find_package(UmicomFramework 0.9 CONFIG REQUIRED)") !=
           NULL);
    assert(strstr(cmake, "Umicom::delivery") != NULL);
    return 0;
}
