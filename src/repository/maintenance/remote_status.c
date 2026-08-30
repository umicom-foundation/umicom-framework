/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/remote_status.c
 *
 * PURPOSE:
 *   Implement repository remote configuration validation.
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
#include "umicom/repository/remote_status.h"

#include <string.h>

void umi_repository_remote_status_init(UmiRepositoryRemoteStatus *status)
{
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

UmiStatus umi_repository_remote_status_validate(const UmiRepositoryRemoteStatus *status)
{
    if (status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if ((status->has_origin || status->upstream_configured || status->fetch_available) &&
        status->remote_count == 0U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
