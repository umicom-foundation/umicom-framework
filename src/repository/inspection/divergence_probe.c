/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/divergence_probe.c
 *
 * PURPOSE:
 *   Implement read ahead/behind counts relative to the configured upstream without network access.
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

#include "umicom/repository/divergence_probe.h"

#include <stdio.h>
#include <string.h>
#include "umicom/repository/git_command.h"
/* Execute rev-list against the already-configured local upstream ref. */
UmiStatus umi_repository_divergence_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryDivergence *out_value)
{
    const char *arguments[] = {"rev-list", "--left-right", "--count", "HEAD...@{u}"};
    UmiRepositoryGitCommandResult result; unsigned long long left = 0U, right = 0U; UmiStatus status;
    if (context == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_value, 0, sizeof(*out_value));
    status = umi_repository_git_command_execute(context, arguments, 4U, &result);
    if (status != UMI_STATUS_OK) return status;
    if (result.exit_code != 0) return UMI_STATUS_NOT_FOUND;
    if (sscanf(result.output, "%llu %llu", &left, &right) != 2) return UMI_STATUS_PARSE_ERROR;
    out_value->ahead = (size_t)left; out_value->behind = (size_t)right; out_value->has_upstream = 1;
    return UMI_STATUS_OK;
}

