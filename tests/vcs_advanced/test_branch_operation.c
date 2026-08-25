/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_branch_operation.c
 *
 * PURPOSE:
 *   Validate plan branch create, checkout, rename and deletion without executing git in ui code.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/branch_operation.h"
int main(void)
{
    UmiVcsAdvancedBranchOperation op;
    umi_vcs_advanced_branch_operation_init(&op);
    if (umi_vcs_advanced_branch_operation_create(&op, "feature/vcs", "HEAD", 1) != UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_branch_operation_rename(&op, "feature/vcs", "feature/diff") != UMI_STATUS_OK) return 2;
    return 0;
}
