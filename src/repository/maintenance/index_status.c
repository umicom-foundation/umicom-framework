/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/index_status.c
 *
 * PURPOSE:
 *   Implement repository index status validation and dirtiness checks.
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
#include "umicom/repository/index_status.h"

#include <string.h>

/*
 * Initialise repository index status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_index_status_init(UmiRepositoryIndexStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

/*
 * Provide the repository index status dirty operation used by this module and its client
 * applications.
 */
int umi_repository_index_status_dirty(const UmiRepositoryIndexStatus *status)
{
    return status != NULL &&
        (status->staged_paths > 0U || status->staged_gitlinks > 0U || status->conflicted_paths > 0U);
}

/*
 * Check that repository index status satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_index_status_validate(const UmiRepositoryIndexStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status->staged_gitlinks > status->staged_paths) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
