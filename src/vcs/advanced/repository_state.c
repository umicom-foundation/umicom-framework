/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/repository_state.c
 *
 * PURPOSE:
 *   Aggregate branch/upstream and in-progress Git operation state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/repository_state.h"

#include <string.h>

void umi_vcs_advanced_repository_state_init(UmiVcsAdvancedRepositoryState *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_repository_state_validate(const UmiVcsAdvancedRepositoryState *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->head_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_repository_state_operation_in_progress(const UmiVcsAdvancedRepositoryState *value)
{
    if (value == NULL) return 0;
    return value->merge_in_progress || value->rebase_in_progress ||
           value->cherry_pick_in_progress || value->revert_in_progress ||
           value->bisect_in_progress;
}
int umi_vcs_advanced_repository_state_diverged(const UmiVcsAdvancedRepositoryState *value)
{
    return value != NULL && value->ahead > 0U && value->behind > 0U;
}
