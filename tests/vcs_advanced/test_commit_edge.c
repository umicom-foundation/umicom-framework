/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_commit_edge.c
 *
 * PURPOSE:
 *   Validate describe a directed parent relationship between history graph commits.
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

int main(void)
{
    UmiVcsAdvancedCommitEdge value;
    umi_vcs_advanced_commit_edge_init(&value);
    if (umi_vcs_advanced_commit_edge_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_commit_edge_set(&value, "child", "parent", 0U) != UMI_STATUS_OK) return 2;
    return 0;
}
