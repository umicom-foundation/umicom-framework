/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/rebase_step.c
 *
 * PURPOSE:
 *   Implement one interactive rebase instruction while preserving commit identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/rebase_step.h"

#include <string.h>

void umi_vcs_advanced_rebase_step_init(UmiVcsAdvancedRebaseStep *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_rebase_step_validate(const UmiVcsAdvancedRebaseStep *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->commit_oid) || value->action > 6U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_rebase_step_set(UmiVcsAdvancedRebaseStep *value,
                                             const char *commit_oid,
                                             const char *subject,
                                             size_t sequence,
                                             UmiVcsAdvancedRebaseAction action)
{
    UmiStatus status;
    if (value == NULL || sequence > UINT32_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->commit_oid, sizeof(value->commit_oid), commit_oid);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->subject, sizeof(value->subject), subject);
    if (status != UMI_STATUS_OK) return status;
    value->sequence = (uint32_t)sequence;
    value->action = (uint32_t)action;
    return umi_vcs_advanced_rebase_step_validate(value);
}
