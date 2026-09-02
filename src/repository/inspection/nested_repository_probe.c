/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/nested_repository_probe.c
 *
 * PURPOSE:
 *   Implement report whether the inspected repository is itself nested inside a Git superproject.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/nested_repository_probe.h"

#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"

/* Ask Git directly for superproject metadata so path heuristics are avoided. */
UmiStatus umi_repository_nested_repository_probe_read(
    const UmiRepositoryInspectionContext *context,
    int *out_nested)
{
    const char *args[] = {"rev-parse", "--show-superproject-working-tree"};
    UmiRepositoryGitCommandResult result;
    UmiStatus status;

    /* Require an output location and initialise it conservatively. */
    if (context == NULL || out_nested == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_nested = 0;

    /* Git returns an empty path when the repository is not a submodule. */
    status = umi_repository_git_command_execute(context, args, 2U, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.exit_code != 0) {
        return UMI_STATUS_OK;
    }

    /* Non-empty superproject output marks this repository as nested. */
    (void)umi_repository_git_output_trim(result.output);
    *out_nested = result.output[0] != '\0';
    return UMI_STATUS_OK;
}
