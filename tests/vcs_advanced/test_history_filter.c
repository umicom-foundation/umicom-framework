/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_history_filter.c
 *
 * PURPOSE:
 *   Validate filter commit-history nodes by author, subject, path/time hints and merge policy.
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
#include "umicom/vcs/advanced/history_filter.h"
int main(void)
{
    UmiVcsAdvancedHistoryFilter f;
    UmiVcsAdvancedCommitNode n;
    umi_vcs_advanced_history_filter_init(&f);
    umi_vcs_advanced_commit_node_init(&n);
    if (umi_vcs_advanced_commit_node_set(&n, "1", "Add Git Graph", "Sammy") != UMI_STATUS_OK) return 1;
    n.timestamp_seconds = 100U;
    if (umi_vcs_advanced_copy_text(f.author_contains, sizeof(f.author_contains), "sam") != UMI_STATUS_OK) return 2;
    if (!umi_vcs_advanced_history_filter_match(&f, &n)) return 3;
    f.merges_only = 1;
    if (umi_vcs_advanced_history_filter_match(&f, &n)) return 4;
    return 0;
}
