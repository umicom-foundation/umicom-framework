/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/fetch_state_probe.c
 *
 * PURPOSE:
 *   Implement report whether the configured upstream ref is available locally without performing a fetch.
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

#include "umicom/repository/fetch_state_probe.h"

#include "umicom/repository/git_command.h"
/* Resolve @{u} locally so repository doctor can distinguish missing upstream metadata. */
UmiStatus umi_repository_fetch_state_probe_read(const UmiRepositoryInspectionContext *context, int *out_available)
{
    const char *args[] = {"rev-parse", "--verify", "@{u}"}; UmiRepositoryGitCommandResult result; UmiStatus status;
    if (context == NULL || out_available == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_git_command_execute(context, args, 3U, &result); if (status != UMI_STATUS_OK) return status;
    *out_available = result.exit_code == 0; return UMI_STATUS_OK;
}

