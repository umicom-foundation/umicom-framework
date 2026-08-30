/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/head_probe.c
 *
 * PURPOSE:
 *   Implement read the repository HEAD object identity without modifying refs.
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

#include "umicom/repository/head_probe.h"
#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"

/* Execute this probe through the single safe Git command boundary. */
UmiStatus umi_repository_head_probe_read(
    const UmiRepositoryInspectionContext *context,
    char *out_value,
    size_t capacity)
{
    const char *arguments[] = {"rev-parse", "HEAD"};
    UmiRepositoryGitCommandResult result;
    UmiStatus status;

    /* Validate the caller-owned destination before invoking Git. */
    if (context == NULL || out_value == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Resolve HEAD without constructing shell command text. */
    status = umi_repository_git_command_execute(
        context, arguments, sizeof(arguments) / sizeof(arguments[0]), &result);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (result.exit_code != 0) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Normalise the captured SHA and copy it through the bounded helper. */
    (void)umi_repository_git_output_trim(result.output);
    return umi_repository_inspection_copy_text(
        out_value, capacity, result.output);
}
