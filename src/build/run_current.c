/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/run_current.c
 * PURPOSE: Resolve explicit launch inputs once for every frontend that runs an existing program.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/run_current.h"
#include "umicom/build/launch_plan.h"
#include "umicom/platform/filesystem.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiBuildRunCurrentPrepare(const UmiBuildProfile *profile, UmiBuildProfile *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    /* These bounded value objects are deliberately heap-owned. Frontends often
     * call from a native event stack, and future profile fields must not turn
     * an ordinary button click into a large stack allocation. */
    UmiBuildProfile *candidate = malloc(sizeof(*candidate));
    UmiBuildLaunchPlan *launch = malloc(sizeof(*launch));
    if (candidate == NULL || launch == NULL)
    {
        free(candidate);
        free(launch);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *candidate = *profile;
    status = UmiBuildProfileSourceDirectory(profile, candidate->source_directory,
                                            sizeof(candidate->source_directory));
    if (status == UMI_STATUS_OK)
        status = UmiBuildLaunchPlanCreate(profile, candidate->source_directory, launch);
    /* Explicit paths prevent a selected build product from silently becoming
     * a similarly named executable found elsewhere on the user's PATH. */
    if (status == UMI_STATUS_OK && launch->run_uses_program_lookup)
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK &&
        (!umi_fs_is_directory(candidate->source_directory) ||
         !umi_fs_is_directory(launch->working_directory) || !umi_fs_is_file(launch->run_program)))
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
    {
        strcpy(candidate->run_program, launch->run_program);
        strcpy(candidate->run_working_directory, launch->working_directory);
        *out = *candidate;
    }
    free(launch);
    free(candidate);
    return status;
}
