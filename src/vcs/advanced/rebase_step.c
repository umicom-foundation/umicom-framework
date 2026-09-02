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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/rebase_step.h"

#include <string.h>

/*
 * Initialise vcs advanced rebase step from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_rebase_step_init(UmiVcsAdvancedRebaseStep *value)
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
 * Check that vcs advanced rebase step satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_rebase_step_validate(const UmiVcsAdvancedRebaseStep *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->commit_oid) || value->action > 6U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced rebase step into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_rebase_step_set(UmiVcsAdvancedRebaseStep *value,
                                             const char *commit_oid,
                                             const char *subject,
                                             size_t sequence,
                                             UmiVcsAdvancedRebaseAction action)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || sequence > UINT32_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->commit_oid, sizeof(value->commit_oid), commit_oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->subject, sizeof(value->subject), subject);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->sequence = (uint32_t)sequence;
    value->action = (uint32_t)action;
    return umi_vcs_advanced_rebase_step_validate(value);
}
