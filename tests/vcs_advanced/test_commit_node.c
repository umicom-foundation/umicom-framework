/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_commit_node.c
 *
 * PURPOSE:
 *   Validate describe one commit in the framework-owned history graph.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/commit_node.h"

int main(void)
{
    UmiVcsAdvancedCommitNode value;
    umi_vcs_advanced_commit_node_init(&value);
    if (umi_vcs_advanced_commit_node_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_commit_node_set(&value, "abc123", "subject", "author") != UMI_STATUS_OK) return 2;
    value.parent_count = 2U;
    value.merge_commit = 1;
    if (umi_vcs_advanced_commit_node_validate(&value) != UMI_STATUS_OK) return 3;
    return 0;
}
