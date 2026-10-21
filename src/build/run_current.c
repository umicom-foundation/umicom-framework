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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Launch review now shares the preparation path and provides field-specific explanations to native frontends. The former preparation body is retained for review; it is replaced without changing its output-on-failure or explicit-path contract. */
#if 0
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

#endif
/* Review and execution share the same resolution rules. Frontends can explain
 * the field that blocks launch without implementing their own path checks. */
static UmiStatus RunCurrentMessage(char *reason, size_t capacity,
    const char *message, UmiStatus status)
{
    if (reason != NULL && capacity != 0U)
        (void)snprintf(reason, capacity, "%s", message);
    return status;
}

static UmiStatus RunCurrentPrepare(const UmiBuildProfile *profile, UmiBuildProfile *out,
    char *reason, size_t capacity)
{
    if ((reason == NULL) != (capacity == 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (reason != NULL) reason[0] = '\0';
    UmiStatus status = umi_build_profile_validate(profile, reason, capacity);
    if (status != UMI_STATUS_OK) {
        if (reason != NULL && reason[0] == '\0')
            (void)RunCurrentMessage(reason, capacity, "Review the project build profile before running.", status);
        return status;
    }
    if (profile->run_program[0] == '\0')
        return RunCurrentMessage(reason, capacity,
            "Select Program to run in Project Build Settings. Run current executable does not choose or build a program.",
            UMI_STATUS_INVALID_STATE);
    /* Preserve the heap ownership used by native event handlers. The review
     * returns only bounded text; it never exposes borrowed profile strings. */
    UmiBuildProfile *candidate = malloc(sizeof(*candidate));
    UmiBuildLaunchPlan *launch = malloc(sizeof(*launch));
    if (candidate == NULL || launch == NULL) {
        free(candidate);
        free(launch);
        return RunCurrentMessage(reason, capacity, "There is not enough memory to review the selected program.",
            UMI_STATUS_OUT_OF_MEMORY);
    }
    *candidate = *profile;
    status = UmiBuildProfileSourceDirectory(profile, candidate->source_directory,
        sizeof(candidate->source_directory));
    if (status != UMI_STATUS_OK)
        (void)RunCurrentMessage(reason, capacity, "The project source folder could not be resolved. Reopen the project.", status);
    else {
        status = UmiBuildLaunchPlanCreate(profile, candidate->source_directory, launch);
        if (status != UMI_STATUS_OK)
            (void)RunCurrentMessage(reason, capacity,
                "Review the program path, working folder and arguments in Project Build Settings.", status);
    }
    if (status == UMI_STATUS_OK && launch->run_uses_program_lookup)
        status = RunCurrentMessage(reason, capacity,
            "Choose an explicit program path, such as ./program or a full file path. Bare program names are not used for this action.",
            UMI_STATUS_INVALID_ARGUMENT);
    if (status == UMI_STATUS_OK && !umi_fs_is_directory(candidate->source_directory))
        status = RunCurrentMessage(reason, capacity,
            "The project source folder is missing or is not a directory. Reopen an existing project folder.", UMI_STATUS_NOT_FOUND);
    if (status == UMI_STATUS_OK && !umi_fs_is_directory(launch->working_directory))
        status = RunCurrentMessage(reason, capacity,
            "The Run working folder is missing or is not a directory. Choose an existing folder in Project Build Settings.", UMI_STATUS_NOT_FOUND);
    if (status == UMI_STATUS_OK && !umi_fs_is_file(launch->run_program))
        status = RunCurrentMessage(reason, capacity,
            "The selected program file is missing or is not a file. Build the project first or select an existing program in Project Build Settings.", UMI_STATUS_NOT_FOUND);
    if (status == UMI_STATUS_OK) {
        strcpy(candidate->run_program, launch->run_program);
        strcpy(candidate->run_working_directory, launch->working_directory);
        if (out != NULL) *out = *candidate;
        (void)RunCurrentMessage(reason, capacity,
            "The program file and working folder exist. No process was started; the launcher still checks whether the program can run.", status);
    }
    free(launch);
    free(candidate);
    return status;
}

UmiStatus UmiBuildRunCurrentPrepare(const UmiBuildProfile *profile, UmiBuildProfile *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return RunCurrentPrepare(profile, out, NULL, 0U);
}

UmiStatus UmiBuildRunCurrentReview(const UmiBuildProfile *profile, char *reason, size_t capacity)
{
    return RunCurrentPrepare(profile, NULL, reason, capacity);
}
