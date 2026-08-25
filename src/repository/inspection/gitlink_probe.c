/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/gitlink_probe.c
 *
 * PURPOSE:
 *   Implement count index gitlinks by delegating record parsing to the established gitlink parser.
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

#include "umicom/repository/gitlink_probe.h"

#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"
#include "umicom/repository/gitlink.h"

/* Request staged index records and reuse the canonical gitlink parser for each line. */
UmiStatus umi_repository_gitlink_probe_read(
    const UmiRepositoryInspectionContext *context,
    size_t *out_count)
{
    const char *args[] = {"ls-files", "--stage"};
    UmiRepositoryGitCommandResult result;
    UmiStatus status;
    size_t count;
    size_t index;
    char line[4096];

    /* Validate caller-owned output before launching Git. */
    if (context == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_count = 0U;

    /* Read the staged index once and reject command failures explicitly. */
    status = umi_repository_git_command_execute(context, args, 2U, &result);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (result.exit_code != 0) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Count only valid stage-zero gitlink records using the established parser. */
    count = umi_repository_git_output_line_count(result.output);
    for (index = 0U; index < count; ++index) {
        UmiRepositoryGitlink gitlink;
        if (umi_repository_git_output_line_at(
                result.output, index, line, sizeof(line)) == UMI_STATUS_OK &&
            umi_repository_gitlink_parse(line, &gitlink) == UMI_STATUS_OK &&
            gitlink.valid && gitlink.stage == 0U) {
            ++*out_count;
        }
    }
    return UMI_STATUS_OK;
}
