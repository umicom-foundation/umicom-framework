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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/index_status.h"

#include <string.h>

void umi_repository_index_status_init(UmiRepositoryIndexStatus *status)
{
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

int umi_repository_index_status_dirty(const UmiRepositoryIndexStatus *status)
{
    return status != NULL &&
        (status->staged_paths > 0U || status->staged_gitlinks > 0U || status->conflicted_paths > 0U);
}

UmiStatus umi_repository_index_status_validate(const UmiRepositoryIndexStatus *status)
{
    if (status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (status->staged_gitlinks > status->staged_paths) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
