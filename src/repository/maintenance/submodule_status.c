/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/submodule_status.c
 *
 * PURPOSE:
 *   Implement submodule consistency checks for repository maintenance.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/submodule_status.h"

#include <string.h>

void umi_repository_submodule_status_init(UmiRepositorySubmoduleStatus *status)
{
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

UmiStatus umi_repository_submodule_status_validate(const UmiRepositorySubmoduleStatus *status)
{
    if (status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (status->initialised > status->configured || status->missing > status->configured ||
        status->dirty > status->configured || status->head_mismatch > status->configured) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

int umi_repository_submodule_status_healthy(const UmiRepositorySubmoduleStatus *status)
{
    return status != NULL && umi_repository_submodule_status_validate(status) == UMI_STATUS_OK &&
           status->initialised + status->missing == status->configured &&
           status->missing == 0U && status->dirty == 0U && status->head_mismatch == 0U;
}
