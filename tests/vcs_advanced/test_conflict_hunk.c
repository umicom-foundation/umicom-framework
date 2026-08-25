/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_conflict_hunk.c
 *
 * PURPOSE:
 *   Validate describe aligned base/ours/theirs ranges for one textual conflict.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/conflict_hunk.h"

int main(void)
{
    UmiVcsAdvancedConflictHunk value;
    umi_vcs_advanced_conflict_hunk_init(&value);
    if (umi_vcs_advanced_conflict_hunk_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_conflict_hunk_set(&value, 1U, 1U, 1U, 1U, 1U, 2U, "conflict") != UMI_STATUS_OK) return 2;
    return 0;
}
