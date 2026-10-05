/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/launch_plan.h
 * PURPOSE: Describe Run and native Debug launch inputs without starting a process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LAUNCH_PLAN_H
#define UMICOM_BUILD_LAUNCH_PLAN_H
#include <stdbool.h>
#include "umicom/build/profile.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* This value owns its strings and has no internal pointers. It can be copied
 * for review without retaining a profile or a parser's temporary storage. */
    typedef struct UmiBuildLaunchPlan
    {
        char run_program[UMI_BUILD_PATH_CAPACITY];
        char debug_program[UMI_BUILD_PATH_CAPACITY];
        char working_directory[UMI_BUILD_PATH_CAPACITY];
        char arguments[UMI_ARGUMENTS_CAPACITY][UMI_BUILD_ARGUMENT_CAPACITY];
        size_t argument_count;
        bool run_uses_program_lookup;
    } UmiBuildLaunchPlan;
    typedef enum UmiBuildLaunchPathKind
    {
        UMI_BUILD_LAUNCH_PROGRAM,
        UMI_BUILD_LAUNCH_DIRECTORY
    } UmiBuildLaunchPathKind;
    /* Resolve against an already absolute project root, never the host's cwd.
 * Preserve legacy lookup: a bare Run name is passed to the process launcher;
 * native Debug interprets it as a project file. Use ./program or an absolute
 * path to select the same file for both. No filesystem inspection, permission
 * change, shell evaluation, setting save or process execution occurs.
 * Windows drive-relative and rooted-without-drive program paths are rejected.
 * Control bytes in paths are rejected to keep review fields unambiguous.
 * Missing program returns INVALID_STATE. Failure leaves out unchanged. */
    UmiStatus UmiBuildLaunchPlanCreate(const UmiBuildProfile *profile, const char *absoluteProjectDirectory,
                                       UmiBuildLaunchPlan *out);
    /* Adopt an explicitly chosen local absolute path into one field only. This
 * does not prove existence, executable format, trust or access permissions.
 * Failure leaves out unchanged. Caller inputs must be terminated strings. */
    UmiStatus UmiBuildLaunchSelectPath(const UmiBuildProfile *profile, UmiBuildLaunchPathKind kind,
                                       const char *absolutePath, UmiBuildProfile *out);
#ifdef __cplusplus
}
#endif
#endif
