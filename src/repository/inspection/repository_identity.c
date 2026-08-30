/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/repository_identity.c
 *
 * PURPOSE:
 *   Implement read canonical repository root and worktree/bare identity.
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

#include "umicom/repository/repository_identity.h"
#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"

/* Execute this probe through the single safe Git command boundary. */
UmiStatus umi_repository_repository_identity_read(
    const UmiRepositoryInspectionContext *context,
    char *out_value,
    size_t capacity)
{
    const char *arguments[] = {"rev-parse", "--show-toplevel"};
    UmiRepositoryGitCommandResult result;
    UmiStatus status;

    /* Validate caller-owned storage before probing repository identity. */
    if (context == NULL || out_value == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Ask Git for its authoritative top-level path rather than deriving it. */
    status = umi_repository_git_command_execute(
        context, arguments, sizeof(arguments) / sizeof(arguments[0]), &result);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (result.exit_code != 0) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Return a trimmed, bounded repository root to the caller. */
    (void)umi_repository_git_output_trim(result.output);
    return umi_repository_inspection_copy_text(
        out_value, capacity, result.output);
}
