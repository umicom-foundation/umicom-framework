/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/types.c
 *
 * PURPOSE:
 *   Implement stable version-control change-state text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/types.h"

const char *umi_vcs_change_state_text(UmiVcsChangeState state)
{
    switch (state) {
        case UMI_VCS_CHANGE_UNMODIFIED: return "unmodified";
        case UMI_VCS_CHANGE_ADDED: return "added";
        case UMI_VCS_CHANGE_MODIFIED: return "modified";
        case UMI_VCS_CHANGE_DELETED: return "deleted";
        case UMI_VCS_CHANGE_RENAMED: return "renamed";
        case UMI_VCS_CHANGE_COPIED: return "copied";
        case UMI_VCS_CHANGE_UNTRACKED: return "untracked";
        case UMI_VCS_CHANGE_IGNORED: return "ignored";
        case UMI_VCS_CHANGE_CONFLICTED: return "conflicted";
        default: return "unknown";
    }
}

const char *umi_vcs_conflict_kind_text(UmiVcsConflictKind kind)
{
    switch (kind) {
        case UMI_VCS_CONFLICT_BOTH_ADDED: return "both added";
        case UMI_VCS_CONFLICT_BOTH_MODIFIED: return "both modified";
        case UMI_VCS_CONFLICT_BOTH_DELETED: return "both deleted";
        case UMI_VCS_CONFLICT_ADDED_BY_US: return "added by us";
        case UMI_VCS_CONFLICT_ADDED_BY_THEM: return "added by them";
        case UMI_VCS_CONFLICT_DELETED_BY_US: return "deleted by us";
        case UMI_VCS_CONFLICT_DELETED_BY_THEM: return "deleted by them";
        case UMI_VCS_CONFLICT_UNKNOWN: return "unknown conflict";
        case UMI_VCS_CONFLICT_NONE: return "none";
        default: return "unknown";
    }
}

const char *umi_vcs_diff_line_kind_text(UmiVcsDiffLineKind kind)
{
    switch (kind) {
        case UMI_VCS_DIFF_ADDITION: return "addition";
        case UMI_VCS_DIFF_DELETION: return "deletion";
        case UMI_VCS_DIFF_HEADER: return "header";
        case UMI_VCS_DIFF_HUNK: return "hunk";
        case UMI_VCS_DIFF_NOTICE: return "notice";
        case UMI_VCS_DIFF_CONTEXT: return "context";
        default: return "unknown";
    }
}

const char *umi_vcs_operation_kind_text(UmiVcsOperationKind kind)
{
    switch (kind) {
        case UMI_VCS_OPERATION_REFRESH: return "refresh";
        case UMI_VCS_OPERATION_STAGE: return "stage";
        case UMI_VCS_OPERATION_UNSTAGE: return "unstage";
        case UMI_VCS_OPERATION_DISCARD: return "discard";
        case UMI_VCS_OPERATION_COMMIT: return "commit";
        case UMI_VCS_OPERATION_FETCH: return "fetch";
        case UMI_VCS_OPERATION_PULL: return "pull";
        case UMI_VCS_OPERATION_PUSH: return "push";
        case UMI_VCS_OPERATION_BRANCH_CREATE: return "create branch";
        case UMI_VCS_OPERATION_BRANCH_CHECKOUT: return "checkout branch";
        case UMI_VCS_OPERATION_BRANCH_DELETE: return "delete branch";
        case UMI_VCS_OPERATION_DIFF: return "diff";
        default: return "unknown";
    }
}

const char *umi_vcs_operation_state_text(UmiVcsOperationState state)
{
    switch (state) {
        case UMI_VCS_OPERATION_PENDING: return "pending";
        case UMI_VCS_OPERATION_RUNNING: return "running";
        case UMI_VCS_OPERATION_SUCCEEDED: return "succeeded";
        case UMI_VCS_OPERATION_FAILED: return "failed";
        case UMI_VCS_OPERATION_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}
