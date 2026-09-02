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

/*
 * Initialise repository branch status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_branch_status_init(UmiRepositoryBranchStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

/*
 * Copy repository branch status into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_repository_branch_status_set(UmiRepositoryBranchStatus *status,
                                           const char *branch,
                                           const char *upstream,
                                           size_t ahead,
                                           size_t behind,
                                           int detached)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL || branch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(branch) >= sizeof(status->branch) ||
        (upstream != NULL && strlen(upstream) >= sizeof(status->upstream))) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_repository_branch_status_init(status);
    (void)snprintf(status->branch, sizeof(status->branch), "%s", branch);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (upstream != NULL) (void)snprintf(status->upstream, sizeof(status->upstream), "%s", upstream);
    status->ahead = ahead; status->behind = behind; status->detached = detached != 0;
    return umi_repository_branch_status_validate(status);
}

/*
 * Check that repository branch status satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_branch_status_validate(const UmiRepositoryBranchStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL || (!status->detached && status->branch[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
