/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/upstream_probe.c
 *
 * PURPOSE:
 *   Implement read the configured upstream ref for the current branch.
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

#include "umicom/repository/upstream_probe.h"
#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"
#include "umicom/repository/porcelain_parser.h"

/* Execute this probe through the single safe Git command boundary. */
UmiStatus umi_repository_upstream_probe_read(const UmiRepositoryInspectionContext *context, char *out_value, size_t capacity)
{
    const char *arguments[] = {"rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}"};
    UmiRepositoryGitCommandResult result;
    UmiStatus status;
    if (context == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_git_command_execute(context, arguments, sizeof(arguments)/sizeof(arguments[0]), &result);
    if (status != UMI_STATUS_OK) return status;
    if (result.exit_code != 0) { if (capacity > 0U) out_value[0] = '\0'; return UMI_STATUS_NOT_FOUND; } (void)umi_repository_git_output_trim(result.output); return umi_repository_inspection_copy_text(out_value, capacity, result.output);
}
