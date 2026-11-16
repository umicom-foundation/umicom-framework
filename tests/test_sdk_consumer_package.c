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


/* Extend the existing active-check fixture with fixed-field boundaries. The
 * shared assertion policy continues to govern the original integration checks. */
static int UmiSdkExportPlanPathCases(void)
{
    UmiSdkExportPlan *value = malloc(sizeof *value);
    unsigned char *before = malloc(sizeof *value);
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, "C:/Umicom/Framework", "0.9") == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_validate(value) == UMI_STATUS_INVALID_STATE);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_add_target(value, "sdk", "Umicom::sdk", 1) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_validate(value) == UMI_STATUS_OK);
    char *fields[] = {value->install_prefix, value->include_directory,
        value->library_directory, value->examples_directory};
    const size_t capacities[] = {sizeof value->install_prefix, sizeof value->include_directory,
        sizeof value->library_directory, sizeof value->examples_directory};
    for (size_t field = 0U; field < sizeof fields / sizeof fields[0]; ++field) {
        for (size_t form = 0U; form < 3U; ++form) {
            /* Rebuild to isolate each field, preserving the initializer's real
             * defaults rather than fabricating a second model implementation. */
            UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, "C:/Umicom/Framework", "0.9") == UMI_STATUS_OK);
            UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_add_target(value, "sdk", "Umicom::sdk", 1) == UMI_STATUS_OK);
            memset(fields[field], 'x', capacities[field]);
            if (form == 1U) fields[field][0] = '\0';
            if (form == 2U) fields[field][capacities[field] - 1U] = '\0';
            memcpy(before, value, sizeof *value);
            UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_validate(value) ==
                (form == 2U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE));
            UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof *value) == 0);
        }
    }
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, "C:/Umicom/Framework", "0.9") == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_add_target(value, "sdk", "Umicom::sdk", 1) == UMI_STATUS_OK);
    memset(value->package.package_name, 'x', sizeof value->package.package_name);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_validate(value) == UMI_STATUS_INVALID_STATE);
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_init(value, "C:/Umicom/Framework", "0.9") == UMI_STATUS_OK);
    value->catalogue.count = UMI_SDK_EXPORT_CAPACITY + 1U;
    UMI_CONSTRUCTION_REQUIRE(umi_sdk_export_plan_validate(value) == UMI_STATUS_INVALID_STATE);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiSdkExportPlanConstructionCases() != 0) return 1;
    if (UmiSdkExportPlanPathCases() != 0) return 1;

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
