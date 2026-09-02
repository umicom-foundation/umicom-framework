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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/recovery_checkpoint.h"

#include <string.h>

/*
 * Initialise vcs advanced recovery checkpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_recovery_checkpoint_init(UmiVcsAdvancedRecoveryCheckpoint *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced recovery checkpoint satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_vcs_advanced_recovery_checkpoint_validate(const UmiVcsAdvancedRecoveryCheckpoint *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->checkpoint_id) || !umi_vcs_advanced_text_present(value->head_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced recovery checkpoint into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_vcs_advanced_recovery_checkpoint_set(UmiVcsAdvancedRecoveryCheckpoint *value,
                                                     const char *checkpoint_id,
                                                     const char *head_oid,
                                                     const char *branch,
                                                     const char *operation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->checkpoint_id, sizeof(value->checkpoint_id), checkpoint_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->head_oid, sizeof(value->head_oid), head_oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->branch, sizeof(value->branch), branch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->operation, sizeof(value->operation), operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_recovery_checkpoint_validate(value);
}
