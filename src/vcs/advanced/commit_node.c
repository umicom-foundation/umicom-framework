/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/commit_node.c
 *
 * PURPOSE:
 *   Implement one commit in the Framework-owned history graph.
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
#include "umicom/vcs/advanced/commit_node.h"

#include <string.h>

/*
 * Initialise vcs advanced commit node from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_node_init(UmiVcsAdvancedCommitNode *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->generation = 1U;
}

/*
 * Check that vcs advanced commit node satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_commit_node_validate(const UmiVcsAdvancedCommitNode *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced commit node into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_commit_node_set(UmiVcsAdvancedCommitNode *value,
                                              const char *oid,
                                              const char *subject,
                                              const char *author)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !umi_vcs_advanced_text_present(oid)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->oid, sizeof(value->oid), oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->subject, sizeof(value->subject), subject);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_copy_text(value->author, sizeof(value->author), author);
}
