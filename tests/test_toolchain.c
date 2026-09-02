/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain.c
 *
 * PURPOSE:
 *   Verify toolchain profile initialisation, environment-plan construction, and
 *   executable discovery without requiring a particular compiler vendor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainProfile profile;
    UmiEnvironmentPlan environment;
    char executable[UMI_PATH_CAPACITY];

    umi_toolchain_profile_init(&profile);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_tool_kind_name(UMI_TOOL_CPACK), "CPack") != 0)
        return EXIT_FAILURE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_tool_kind_name(UMI_TOOL_CPPCHECK), "Cppcheck") != 0 ||
        strcmp(umi_tool_kind_name(UMI_TOOL_OSV_SCANNER), "OSV-Scanner") != 0 ||
        strcmp(umi_tool_kind_name(UMI_TOOL_VALGRIND), "Valgrind") != 0 ||
        strcmp(umi_tool_kind_name(UMI_TOOL_DR_MEMORY), "Dr. Memory") != 0)
        return EXIT_FAILURE;
#ifdef _WIN32
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_tool_default_executable(UMI_TOOL_CPACK), "cpack.exe") != 0)
        return EXIT_FAILURE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_tool_default_executable(UMI_TOOL_OSV_SCANNER),
               "osv-scanner.exe") != 0)
        return EXIT_FAILURE;
#else
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_tool_default_executable(UMI_TOOL_CPACK), "cpack") != 0)
        return EXIT_FAILURE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_tool_default_executable(UMI_TOOL_OSV_SCANNER),
               "osv-scanner") != 0)
        return EXIT_FAILURE;
#endif
    (void)snprintf(profile.profile_id,
                   sizeof(profile.profile_id),
                   "%s",
                   "test-profile");
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(profile.profile_id, "test-profile") != 0)
        return EXIT_FAILURE;
    umi_environment_plan_init(&environment);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_environment_plan_add(&environment,
                                 "UMICOM_TEST_VALUE",
                                 "ready") != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (environment.count != 1U ||
        strcmp(environment.entries[0].value, "ready") != 0)
        return EXIT_FAILURE;
#ifdef _WIN32
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_find_on_path("cmd.exe",
                                   executable,
                                   sizeof(executable)) != UMI_STATUS_OK)
        return EXIT_FAILURE;
#else
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_find_on_path("sh",
                                   executable,
                                   sizeof(executable)) != UMI_STATUS_OK)
        return EXIT_FAILURE;
#endif
    return EXIT_SUCCESS;
}
