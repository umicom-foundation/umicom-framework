/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/remote_probe.c
 *
 * PURPOSE:
 *   Implement populate existing remote maintenance state from local Git remote configuration.
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

#include "umicom/repository/remote_probe.h"

#include <string.h>
#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"
/* Inspect configured remote names and origin URL without contacting the network. */
UmiStatus umi_repository_remote_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryRemoteStatus *out_value)
{
    const char *list_args[] = {"remote"}; const char *url_args[] = {"remote", "get-url", "origin"};
    UmiRepositoryGitCommandResult result; UmiStatus status; size_t index, count; char line[512];
    if (context == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_remote_status_init(out_value);
    status = umi_repository_git_command_execute(context, list_args, 1U, &result); if (status != UMI_STATUS_OK) return status;
    if (result.exit_code != 0) return UMI_STATUS_INVALID_STATE;
    count = umi_repository_git_output_line_count(result.output); out_value->remote_count = count;
    for (index = 0U; index < count; ++index) {
        if (umi_repository_git_output_line_at(result.output, index, line, sizeof(line)) == UMI_STATUS_OK && strcmp(line, "origin") == 0) out_value->has_origin = 1;
    }
    out_value->upstream_configured = 0;
    if (umi_repository_git_command_execute(context, url_args, 3U, &result) == UMI_STATUS_OK && result.exit_code == 0) out_value->fetch_available = 1;
    return UMI_STATUS_OK;
}

