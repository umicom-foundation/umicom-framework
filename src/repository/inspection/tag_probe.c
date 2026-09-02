/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/tag_probe.c
 *
 * PURPOSE:
 *   Implement read an exact tag attached to HEAD when one exists.
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

#include "umicom/repository/tag_probe.h"
#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"
#include "umicom/repository/porcelain_parser.h"

/* Execute this probe through the single safe Git command boundary. */
UmiStatus umi_repository_tag_probe_read(const UmiRepositoryInspectionContext *context, char *out_value, size_t capacity)
{
    const char *arguments[] = {"describe", "--tags", "--exact-match", "HEAD"};
    UmiRepositoryGitCommandResult result;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_git_command_execute(context, arguments, sizeof(arguments)/sizeof(arguments[0]), &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result.exit_code != 0) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (capacity > 0U) out_value[0] = '\0'; return UMI_STATUS_NOT_FOUND; } (void)umi_repository_git_output_trim(result.output); return umi_repository_inspection_copy_text(out_value, capacity, result.output);
}
