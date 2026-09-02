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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/repository_state.h"

#include <string.h>

/*
 * Initialise vcs advanced repository state from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_repository_state_init(UmiVcsAdvancedRepositoryState *value)
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
 * Check that vcs advanced repository state satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_repository_state_validate(const UmiVcsAdvancedRepositoryState *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->head_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced repository state operation in progress operation used by this
 * module and its client applications.
 */
int umi_vcs_advanced_repository_state_operation_in_progress(const UmiVcsAdvancedRepositoryState *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0;
    return value->merge_in_progress || value->rebase_in_progress ||
           value->cherry_pick_in_progress || value->revert_in_progress ||
           value->bisect_in_progress;
}
/*
 * Provide the vcs advanced repository state diverged operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_repository_state_diverged(const UmiVcsAdvancedRepositoryState *value)
{
    return value != NULL && value->ahead > 0U && value->behind > 0U;
}
