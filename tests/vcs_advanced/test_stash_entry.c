/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_stash_entry.c
 *
 * PURPOSE:
 *   Validate describe one stash entry with base/index/worktree identity and metadata.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/stash_entry.h"

int main(void)
{
    UmiVcsAdvancedStashEntry value;
    umi_vcs_advanced_stash_entry_init(&value);
    if (umi_vcs_advanced_stash_entry_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_stash_entry_set(&value, 0U, "stash@{0}", "abc", "WIP", "main") != UMI_STATUS_OK) return 2;
    return 0;
}
