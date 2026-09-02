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
#include <string.h>
#include "umicom/sdk/sdk.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
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
