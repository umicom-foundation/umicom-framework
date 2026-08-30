/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/submodule_probe.c
 *
 * PURPOSE:
 *   Implement populate existing submodule maintenance counts from recursive submodule status.
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

#include "umicom/repository/submodule_probe.h"

#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"
/* Parse Git's leading submodule state marker and count only configured entries. */
UmiStatus umi_repository_submodule_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositorySubmoduleStatus *out_value)
{
    const char *args[] = {"submodule", "status", "--recursive"}; UmiRepositoryGitCommandResult result; UmiStatus status; size_t count, index; char line[4096];
    if (context == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_submodule_status_init(out_value);
    status = umi_repository_git_command_execute(context, args, 3U, &result); if (status != UMI_STATUS_OK) return status;
    if (result.exit_code != 0 && result.output[0] == '\0') return UMI_STATUS_INVALID_STATE;
    count = umi_repository_git_output_line_count(result.output); out_value->configured = count;
    for (index = 0U; index < count; ++index) {
        if (umi_repository_git_output_line_at(result.output, index, line, sizeof(line)) != UMI_STATUS_OK || line[0] == '\0') continue;
        if (line[0] == '-') ++out_value->missing;
        else { ++out_value->initialised; if (line[0] == '+') ++out_value->head_mismatch; if (line[0] == 'U') ++out_value->dirty; }
    }
    return UMI_STATUS_OK;
}

