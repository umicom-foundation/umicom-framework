/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/ninja_provider.c
 *
 * PURPOSE:
 *   Create direct Ninja build and clean command records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/tool_location.h"
#include "umicom/build/ninja_provider.h"

#include <stdio.h>

static UmiStatus ninja_command(const UmiBuildProfile *profile,
                               UmiBuildPhase phase,
                               UmiBuildCommand *out_command)
{
    char jobs[64];
    if (profile == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (phase != UMI_BUILD_PHASE_BUILD &&
        phase != UMI_BUILD_PHASE_CLEAN) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
/* An explicit project tool folder selects this program directly. The original inherited-lookup call remains for review; empty tool folders preserve that behaviour. The previous implementation is retained for engineering review. */
#if 0
    umi_build_command_init(out_command, "ninja");
#endif
    char tool[UMI_BUILD_PATH_CAPACITY];
    UmiStatus tool_status = UmiBuildToolProgram(profile, "ninja", tool, sizeof tool);
    if (tool_status != UMI_STATUS_OK) return tool_status;
    umi_build_command_init(out_command, tool);
    if (!umi_build_command_set_working_directory(out_command,
                                                  profile->build_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(jobs, sizeof(jobs), "-j%u", profile->parallel_jobs);
    if (!umi_build_command_add_argument(out_command, jobs)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (phase == UMI_BUILD_PHASE_CLEAN &&
        !umi_build_command_add_argument(out_command, "clean")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiBuildProvider umi_build_ninja_provider(void)
{
    UmiBuildProvider provider;
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.provider_id = "umicom.build.ninja";
    provider.supported_phases =
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_BUILD) |
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_CLEAN);
    provider.create_command = ninja_command;
    return provider;
}
