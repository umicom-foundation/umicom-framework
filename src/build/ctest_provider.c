/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/ctest_provider.c
 *
 * PURPOSE:
 *   Create a deterministic CTest command for a selected build profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

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
    umi_build_command_init(out_command, "ctest");
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
