/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/status_probe.c
 *
 * PURPOSE:
 *   Implement the shared porcelain-v2 status probe used by repository inspection projections.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/status_probe.h"

#include "umicom/repository/git_command.h"

/* Execute one read-only status command and project its stable porcelain-v2 output. */
UmiStatus umi_repository_status_probe_read(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryPorcelainStatus *out_status)
{
    const char *arguments[] = {
        "status",
        "--porcelain=v2",
        "--branch",
        "--untracked-files=all"
    };
    UmiRepositoryGitCommandResult result;
    UmiStatus status;

    /* Require caller-owned result storage before launching the Git process. */
    if (context == NULL || out_status == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the single safe argument-based Git execution boundary. */
    status = umi_repository_git_command_execute(
        context,
        arguments,
        sizeof(arguments) / sizeof(arguments[0]),
        &result);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (result.exit_code != 0) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Parse once so all derived status views share identical evidence. */
    return umi_repository_porcelain_parse(result.output, out_status);
}
