/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_staging_hunk.c
 *
 * PURPOSE:
 *   Validate describe a selectable diff hunk for partial staging and unstaging.
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
#include "umicom/vcs/advanced/staging_hunk.h"

int main(void)
{
    UmiVcsAdvancedStagingHunk value;
    umi_vcs_advanced_staging_hunk_init(&value);
    if (umi_vcs_advanced_staging_hunk_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_staging_hunk_set(&value, "src/a.c", 1U, 2U, 1U, 3U, "@@") != UMI_STATUS_OK) return 2;
    if (value.selected == 0) return 3;
    return 0;
}
