/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/prepared_shell.c
 *
 * PURPOSE:
 *   Validate a prepared shell environment and preserve transport and child failure results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build.h"
#include <stdlib.h>
#include <string.h>

UmiStatus umi_build_open_shell(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, const char *working_directory,
    const char *shell_program)
{
    (void)profile;
    /* Validate the owned environment before projecting borrowed process entries.
     * The shared environment service owns count, text and duplicate-name rules. */
    UmiStatus status = umi_environment_plan_validate(environment);
    if (status != UMI_STATUS_OK) return status;
    const char *program = shell_program;
#ifdef _WIN32
    if (!program || !program[0]) program = "powershell.exe";
#else
    if (!program || !program[0]) {
        program = getenv("SHELL");
        if (!program || !program[0]) program = "/bin/sh";
    }
#endif
    UmiProcessRequest request = {0};
    UmiProcessResult result = {0};
    request.program = program;
    request.working_directory = working_directory;
    request.environment = umi_environment_plan_variables(environment);
    request.environment_count = environment->count;
    request.window_mode = UMI_PROCESS_WINDOW_VISIBLE;
    status = umi_process_execute(&request, &result);
    /* Successful transport means the child was observed. It does not imply
     * that the shell exited successfully; preserve the more specific causes. */
    if (status != UMI_STATUS_OK) return status;
    if (result.cancelled) return UMI_STATUS_CANCELLED;
    if (result.timed_out) return UMI_STATUS_TIMEOUT;
    if (!result.launched || result.exit_code != 0) return UMI_STATUS_INTERNAL_ERROR;
    return UMI_STATUS_OK;
}
