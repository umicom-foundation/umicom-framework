/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/recovery_checkpoint.c
 *
 * PURPOSE:
 *   Capture reversible repository identity before risky VCS operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/recovery_checkpoint.h"

#include <string.h>

void umi_vcs_advanced_recovery_checkpoint_init(UmiVcsAdvancedRecoveryCheckpoint *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_recovery_checkpoint_validate(const UmiVcsAdvancedRecoveryCheckpoint *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->checkpoint_id) || !umi_vcs_advanced_text_present(value->head_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_recovery_checkpoint_set(UmiVcsAdvancedRecoveryCheckpoint *value,
                                                     const char *checkpoint_id,
                                                     const char *head_oid,
                                                     const char *branch,
                                                     const char *operation)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->checkpoint_id, sizeof(value->checkpoint_id), checkpoint_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->head_oid, sizeof(value->head_oid), head_oid);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->branch, sizeof(value->branch), branch);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->operation, sizeof(value->operation), operation);
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_recovery_checkpoint_validate(value);
}
