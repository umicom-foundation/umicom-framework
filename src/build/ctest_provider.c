/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/ctest_provider.c
 *
 * PURPOSE:
 *   Create a deterministic CTest command for a selected build profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/tool_location.h"
#include "umicom/build/ctest_provider.h"

static UmiStatus ctest_command(const UmiBuildProfile *profile,
                               UmiBuildPhase phase,
                               UmiBuildCommand *out_command)
{
    if (profile == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (phase != UMI_BUILD_PHASE_TEST) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    UmiStatus validation = umi_build_profile_validate(profile, NULL, 0U);
    if (validation != UMI_STATUS_OK) return validation;
/* An explicit project tool folder selects this program directly. The original inherited-lookup call remains for review; empty tool folders preserve that behaviour. The previous implementation is retained for engineering review. */
#if 0
    umi_build_command_init(out_command, "ctest");
#endif
    char tool[UMI_BUILD_PATH_CAPACITY];
    UmiStatus tool_status = UmiBuildToolProgram(profile, "ctest", tool, sizeof tool);
    if (tool_status != UMI_STATUS_OK) return tool_status;
    umi_build_command_init(out_command, tool);
    /* An explicit test preset supplies its own configuration, environment,
     * filters and execution options. Keep the established visible-failure and
     * nonempty-suite policy without overriding the preset's configuration. */
    if (profile->test_preset[0] != '\0') {
        return umi_build_command_add_argument(out_command, "--preset") &&
               umi_build_command_add_argument(out_command, profile->test_preset) &&
               umi_build_command_add_argument(out_command, "--output-on-failure") &&
               umi_build_command_add_argument(out_command, "--no-tests=error")
            ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* An empty test tree is not a passed test run. */
    if (!umi_build_command_add_argument(out_command, "--no-tests=error") ||
        !umi_build_command_add_argument(out_command, "--build-config") ||
        !umi_build_command_add_argument(out_command, profile->configuration))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (profile->preset[0] != '\0') {
        if (!umi_build_command_add_argument(out_command, "--preset") ||
            !umi_build_command_add_argument(out_command, profile->preset) ||
            !umi_build_command_add_argument(out_command,
                                            "--output-on-failure")) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        return UMI_STATUS_OK;
    }
    if (!umi_build_command_add_argument(out_command, "--test-dir") ||
        !umi_build_command_add_argument(out_command,
                                        profile->build_directory) ||
        !umi_build_command_add_argument(out_command,
                                        "--output-on-failure")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiBuildProvider umi_build_ctest_provider(void)
{
    UmiBuildProvider provider;
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.provider_id = "umicom.build.ctest";
    provider.supported_phases =
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_TEST);
    provider.create_command = ctest_command;
    return provider;
}
