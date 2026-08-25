/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_commit_selection.c
 *
 * PURPOSE:
 *   Validate maintain ordered, de-duplicated commit selections for history operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/commit_selection.h"
int main(void){UmiVcsAdvancedCommitSelection s;
    umi_vcs_advanced_commit_selection_init(&s);
if(umi_vcs_advanced_commit_selection_add(&s,"a")!=UMI_STATUS_OK)return 1;
    if (!umi_vcs_advanced_commit_selection_contains(&s,"a"))return 2;
if(umi_vcs_advanced_commit_selection_remove(&s,"a")!=UMI_STATUS_OK||s.count!=0U)return 3;
    return 0;
}
