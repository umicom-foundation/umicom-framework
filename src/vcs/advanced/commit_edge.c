/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/commit_edge.c
 *
 * PURPOSE:
 *   Implement a directed parent relationship between history graph commits.
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
#include "umicom/vcs/advanced/commit_edge.h"

#include <string.h>

/*
 * Initialise vcs advanced commit edge from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_edge_init(UmiVcsAdvancedCommitEdge *value)
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
 * Check that vcs advanced commit edge satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_commit_edge_validate(const UmiVcsAdvancedCommitEdge *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->child_oid) || !umi_vcs_advanced_text_present(value->parent_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced commit edge into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_commit_edge_set(UmiVcsAdvancedCommitEdge *value,
                                              const char *child_oid,
                                              const char *parent_oid,
                                              uint32_t parent_index)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->child_oid, sizeof(value->child_oid), child_oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->parent_oid, sizeof(value->parent_oid), parent_oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->parent_index = parent_index;
    return umi_vcs_advanced_commit_edge_validate(value);
}
