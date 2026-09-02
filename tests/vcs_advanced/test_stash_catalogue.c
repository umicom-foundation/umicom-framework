/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_stash_catalogue.c
 *
 * PURPOSE:
 *   Validate maintain a bounded, ordered stash catalogue for source control centre.
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
#include "umicom/vcs/advanced/stash_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedStashCatalogue c; UmiVcsAdvancedStashEntry e;
    umi_vcs_advanced_stash_catalogue_init(&c); umi_vcs_advanced_stash_entry_init(&e);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_entry_set(&e, 0U, "stash@{0}", "a", "WIP", "main") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_catalogue_add(&c, &e) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_vcs_advanced_stash_catalogue_find_ref(&c, "stash@{0}") == NULL) return 3;
    return 0;
}
