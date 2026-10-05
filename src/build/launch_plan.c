/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/launch_plan.c
 * PURPOSE: Centralise launch resolution while preserving existing program lookup semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/launch_plan.h"
#include "umicom/platform/path.h"
#include <string.h>

/* Native file selections are explicit absolute paths. Reject control bytes
 * rather than render them as misleading new lines in a settings summary. */
static UmiStatus LaunchChosenPath(const char *path)
{
    if (path == NULL || !umi_path_is_absolute(path))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(path);
    if (length >= UMI_BUILD_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < length; ++i)
        if ((unsigned char)path[i] < 0x20U || (unsigned char)path[i] == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildLaunchPlanCreate(const UmiBuildProfile *profile, const char *absoluteProjectDirectory,
                                   UmiBuildLaunchPlan *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = LaunchChosenPath(absoluteProjectDirectory);
    if (status == UMI_STATUS_OK)
        status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    const char *paths[] = {profile->run_program, profile->run_working_directory};
    for (size_t field = 0U; field < 2U; ++field)
        for (const unsigned char *byte = (const unsigned char *)paths[field]; *byte != 0U; ++byte)
            if (*byte < 0x20U || *byte == 0x7fU)
                return UMI_STATUS_INVALID_ARGUMENT;
    if (profile->run_program[0] == '\0')
        return UMI_STATUS_INVALID_STATE;
#ifdef _WIN32
    /* A drive-relative spelling would borrow hidden process state. A full
     * drive/network path or an ordinary project-relative name is unambiguous. */
    if (!umi_path_is_absolute(profile->run_program) &&
        (profile->run_program[0] == '/' || profile->run_program[0] == '\\' ||
         strchr(profile->run_program, ':') != NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    UmiBuildLaunchPlan candidate = {0};
    status = umi_path_absolute(profile->run_program, absoluteProjectDirectory, candidate.debug_program,
                               sizeof(candidate.debug_program));
    if (status == UMI_STATUS_OK)
        status =
            UmiBuildProfileLaunchDirectory(profile, absoluteProjectDirectory, candidate.working_directory,
                                           sizeof(candidate.working_directory));
    if (status != UMI_STATUS_OK)
        return status;
    candidate.run_uses_program_lookup =
        strchr(profile->run_program, '/') == NULL && strchr(profile->run_program, '\\') == NULL;
    const char *run = candidate.run_uses_program_lookup ? profile->run_program : candidate.debug_program;
    memcpy(candidate.run_program, run, strlen(run) + 1U);
    UmiArguments arguments;
    status = UmiBuildProfileArguments(profile, &arguments);
    if (status != UMI_STATUS_OK)
        return status;
    candidate.argument_count = arguments.count;
    /* Copy each value, not UmiArguments' borrowed pointers. This keeps previews
     * and debugger requests valid after the parser's stack frame returns. */
    for (size_t i = 0U; i < arguments.count; ++i)
        memcpy(candidate.arguments[i], arguments.values[i], strlen(arguments.values[i]) + 1U);
    *out = candidate;
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildLaunchSelectPath(const UmiBuildProfile *profile, UmiBuildLaunchPathKind kind,
                                   const char *absolutePath, UmiBuildProfile *out)
{
    if (out == NULL || (kind != UMI_BUILD_LAUNCH_PROGRAM && kind != UMI_BUILD_LAUNCH_DIRECTORY))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status == UMI_STATUS_OK)
        status = LaunchChosenPath(absolutePath);
    if (status != UMI_STATUS_OK)
        return status;
    UmiBuildProfile candidate = *profile;
    char *field = kind == UMI_BUILD_LAUNCH_PROGRAM ? candidate.run_program : candidate.run_working_directory;
    memcpy(field, absolutePath, strlen(absolutePath) + 1U);
    *out = candidate;
    return UMI_STATUS_OK;
}
