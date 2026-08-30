/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/branch_status.c
 *
 * PURPOSE:
 *   Implement branch status construction and bounded validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/branch_status.h"

#include <stdio.h>
#include <string.h>

void umi_repository_branch_status_init(UmiRepositoryBranchStatus *status)
{
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

UmiStatus umi_repository_branch_status_set(UmiRepositoryBranchStatus *status,
                                           const char *branch,
                                           const char *upstream,
                                           size_t ahead,
                                           size_t behind,
                                           int detached)
{
    if (status == NULL || branch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(branch) >= sizeof(status->branch) ||
        (upstream != NULL && strlen(upstream) >= sizeof(status->upstream))) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_repository_branch_status_init(status);
    (void)snprintf(status->branch, sizeof(status->branch), "%s", branch);
    if (upstream != NULL) (void)snprintf(status->upstream, sizeof(status->upstream), "%s", upstream);
    status->ahead = ahead; status->behind = behind; status->detached = detached != 0;
    return umi_repository_branch_status_validate(status);
}

UmiStatus umi_repository_branch_status_validate(const UmiRepositoryBranchStatus *status)
{
    if (status == NULL || (!status->detached && status->branch[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
