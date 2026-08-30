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

int main(void)
{
    UmiToolchainProfile profile;
    UmiEnvironmentPlan environment;
    char executable[UMI_PATH_CAPACITY];

    umi_toolchain_profile_init(&profile);
    if (strcmp(umi_tool_kind_name(UMI_TOOL_CPACK), "CPack") != 0)
        return EXIT_FAILURE;
#ifdef _WIN32
    if (strcmp(umi_tool_default_executable(UMI_TOOL_CPACK), "cpack.exe") != 0)
        return EXIT_FAILURE;
#else
    if (strcmp(umi_tool_default_executable(UMI_TOOL_CPACK), "cpack") != 0)
        return EXIT_FAILURE;
#endif
    (void)snprintf(profile.profile_id,
                   sizeof(profile.profile_id),
                   "%s",
                   "test-profile");
    if (strcmp(profile.profile_id, "test-profile") != 0)
        return EXIT_FAILURE;
    umi_environment_plan_init(&environment);
    if (umi_environment_plan_add(&environment,
                                 "UMICOM_TEST_VALUE",
                                 "ready") != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (environment.count != 1U ||
        strcmp(environment.entries[0].value, "ready") != 0)
        return EXIT_FAILURE;
#ifdef _WIN32
    if (umi_toolchain_find_on_path("cmd.exe",
                                   executable,
                                   sizeof(executable)) != UMI_STATUS_OK)
        return EXIT_FAILURE;
#else
    if (umi_toolchain_find_on_path("sh",
                                   executable,
                                   sizeof(executable)) != UMI_STATUS_OK)
        return EXIT_FAILURE;
#endif
    return EXIT_SUCCESS;
}
