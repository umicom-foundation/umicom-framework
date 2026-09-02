/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/advanced_service.h
 *
 * PURPOSE:
 *   Compute Source Control Centre readiness and recommend safe next actions from repository state.
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
#ifndef UMICOM_VCS_ADVANCED_ADVANCED_SERVICE_H
#define UMICOM_VCS_ADVANCED_ADVANCED_SERVICE_H
#include "umicom/vcs/advanced/repository_state.h"
#include "umicom/vcs/advanced/worktree_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named vcs advanced recommended action values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedRecommendedAction {
    UMI_VCS_RECOMMEND_NONE=0, UMI_VCS_RECOMMEND_RESOLVE_CONFLICTS=1, UMI_VCS_RECOMMEND_CONTINUE_OPERATION=2,
    UMI_VCS_RECOMMEND_COMMIT=3, UMI_VCS_RECOMMEND_PULL=4, UMI_VCS_RECOMMEND_PUSH=5, UMI_VCS_RECOMMEND_SYNC_REVIEW=6
} UmiVcsAdvancedRecommendedAction;
/**
 * Represent the vcs advanced service snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedServiceSnapshot {
    uint32_t struct_size; uint32_t api_version; UmiVcsAdvancedState state;
    UmiVcsAdvancedRecommendedAction recommended_action; size_t pending_changes,conflicts,ahead,behind;
    int safe_for_history_rewrite;
} UmiVcsAdvancedServiceSnapshot;
/**
 * Provide the vcs advanced service evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_service_evaluate(const UmiVcsAdvancedRepositoryState *repository,
                                             const UmiVcsAdvancedWorktreeSnapshot *worktree,
                                             UmiVcsAdvancedServiceSnapshot *out_snapshot);
/**
 * Provide the vcs advanced recommended action text operation used by this module and its
 * client applications.
 */
const char *umi_vcs_advanced_recommended_action_text(UmiVcsAdvancedRecommendedAction action);
#ifdef __cplusplus
}
#endif
#endif
