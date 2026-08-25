/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_rebase_step.c
 *
 * PURPOSE:
 *   Validate describe one interactive rebase instruction while preserving commit identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/rebase_step.h"

int main(void)
{
    UmiVcsAdvancedRebaseStep value;
    umi_vcs_advanced_rebase_step_init(&value);
    if (umi_vcs_advanced_rebase_step_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_rebase_step_set(&value, "a1", "change", 1U, UMI_VCS_REBASE_SQUASH) != UMI_STATUS_OK) return 2;
    return 0;
}
