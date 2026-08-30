/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_advanced_service.c
 *
 * PURPOSE:
 *   Validate compute source control centre readiness and recommend safe next actions from repository state.
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
#include "umicom/vcs/advanced/advanced_service.h"
int main(void){UmiVcsAdvancedRepositoryState r;UmiVcsAdvancedWorktreeSnapshot w;UmiVcsAdvancedServiceSnapshot s;
umi_vcs_advanced_repository_state_init(&r);
    umi_vcs_advanced_worktree_snapshot_init(&w);
if(umi_vcs_advanced_copy_text(r.head_oid,sizeof(r.head_oid),"a")!=UMI_STATUS_OK)return 1;
if (umi_vcs_advanced_copy_text(w.head_oid, sizeof(w.head_oid), "a") != UMI_STATUS_OK) return 2;
    r.ahead = 1U;
if(umi_vcs_advanced_service_evaluate(&r,&w,&s)!=UMI_STATUS_OK)return 3;
if(s.recommended_action!=UMI_VCS_RECOMMEND_PUSH||!s.safe_for_history_rewrite)return 4;
    return 0;
}
