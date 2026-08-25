/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_staging_selection.c
 *
 * PURPOSE:
 *   Validate represent file, hunk or line scope for partial staging operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/staging_selection.h"

int main(void)
{
    UmiVcsAdvancedStagingSelection value;
    umi_vcs_advanced_staging_selection_init(&value);
    if (umi_vcs_advanced_staging_selection_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_staging_selection_lines(&value, "a.c", 1U, 4U, 8U) != UMI_STATUS_OK) return 2;
    return 0;
}
