/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_recovery_checkpoint.c
 *
 * PURPOSE:
 *   Validate capture reversible repository identity before risky vcs operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/recovery_checkpoint.h"

int main(void)
{
    UmiVcsAdvancedRecoveryCheckpoint value;
    umi_vcs_advanced_recovery_checkpoint_init(&value);
    if (umi_vcs_advanced_recovery_checkpoint_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_recovery_checkpoint_set(&value, "cp1", "abc", "main", "rebase") != UMI_STATUS_OK) return 2;
    return 0;
}
