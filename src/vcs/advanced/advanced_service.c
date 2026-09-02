/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/advanced_service.c
 *
 * PURPOSE:
 *   Implement Source Control Centre readiness and safe recommended actions.
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

#include <string.h>

/*
 * Provide the vcs advanced service evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_service_evaluate(
    const UmiVcsAdvancedRepositoryState *repository,
    const UmiVcsAdvancedWorktreeSnapshot *worktree,
    UmiVcsAdvancedServiceSnapshot *out_snapshot)
{
    /* Apply this operation only while the related capability or state is available. */
    if (umi_vcs_advanced_repository_state_validate(repository) !=
            UMI_STATUS_OK ||
        umi_vcs_advanced_worktree_snapshot_validate(worktree) !=
            UMI_STATUS_OK ||
        out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_VCS_ADVANCED_API_VERSION;
    out_snapshot->conflicts =
        repository->conflicts + worktree->conflict_count;
    out_snapshot->pending_changes =
        worktree->staged_count +
        worktree->modified_count +
        worktree->untracked_count;
    out_snapshot->ahead = repository->ahead;
    out_snapshot->behind = repository->behind;

    /* Apply this branch only when its contract condition is satisfied. */
    if (out_snapshot->conflicts > 0U) {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_CONFLICTED;
        out_snapshot->recommended_action =
            UMI_VCS_RECOMMEND_RESOLVE_CONFLICTS;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (umi_vcs_advanced_repository_state_operation_in_progress(
                   repository)) {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_PENDING;
        out_snapshot->recommended_action =
            UMI_VCS_RECOMMEND_CONTINUE_OPERATION;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (out_snapshot->pending_changes > 0U) {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_READY;
        out_snapshot->recommended_action = UMI_VCS_RECOMMEND_COMMIT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (repository->ahead > 0U && repository->behind > 0U) {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_BLOCKED;
        out_snapshot->recommended_action = UMI_VCS_RECOMMEND_SYNC_REVIEW;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (repository->behind > 0U) {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_READY;
        out_snapshot->recommended_action = UMI_VCS_RECOMMEND_PULL;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (repository->ahead > 0U) {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_READY;
        out_snapshot->recommended_action = UMI_VCS_RECOMMEND_PUSH;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_snapshot->state = UMI_VCS_ADVANCED_STATE_COMPLETED;
        out_snapshot->recommended_action = UMI_VCS_RECOMMEND_NONE;
    }

    out_snapshot->safe_for_history_rewrite =
        out_snapshot->conflicts == 0U &&
        out_snapshot->pending_changes == 0U &&
        repository->behind == 0U &&
        !umi_vcs_advanced_repository_state_operation_in_progress(repository);
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced recommended action text operation used by this module and its
 * client applications.
 */
const char *umi_vcs_advanced_recommended_action_text(
    UmiVcsAdvancedRecommendedAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_VCS_RECOMMEND_NONE:
            return "none";
        case UMI_VCS_RECOMMEND_RESOLVE_CONFLICTS:
            return "resolve-conflicts";
        case UMI_VCS_RECOMMEND_CONTINUE_OPERATION:
            return "continue-operation";
        case UMI_VCS_RECOMMEND_COMMIT:
            return "commit";
        case UMI_VCS_RECOMMEND_PULL:
            return "pull";
        case UMI_VCS_RECOMMEND_PUSH:
            return "push";
        case UMI_VCS_RECOMMEND_SYNC_REVIEW:
            return "sync-review";
        default:
            return "invalid";
    }
}
