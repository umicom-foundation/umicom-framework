/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/types.c
 *
 * PURPOSE:
 *   Implement bounded text helpers, fingerprints and stable enum text.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/types.h"

#include <string.h>

UmiStatus umi_vcs_advanced_copy_text(char *destination,
                                     size_t capacity,
                                     const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source == NULL) {
        source = "";
    }
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

uint64_t umi_vcs_advanced_hash_text(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    uint64_t hash = UINT64_C(1469598103934665603);
    while (*cursor != 0U) {
        hash ^= (uint64_t)(*cursor);
        hash *= UINT64_C(1099511628211);
        cursor += 1;
    }
    return hash;
}

int umi_vcs_advanced_text_present(const char *text)
{
    return text != NULL && text[0] != '\0';
}

int umi_vcs_advanced_text_equal(const char *left, const char *right)
{
    if (left == NULL || right == NULL) {
        return left == right;
    }
    return strcmp(left, right) == 0;
}

const char *umi_vcs_advanced_state_text(UmiVcsAdvancedState state)
{
    switch (state) {
        case UMI_VCS_ADVANCED_STATE_UNKNOWN: return "unknown";
        case UMI_VCS_ADVANCED_STATE_READY: return "ready";
        case UMI_VCS_ADVANCED_STATE_PENDING: return "pending";
        case UMI_VCS_ADVANCED_STATE_BLOCKED: return "blocked";
        case UMI_VCS_ADVANCED_STATE_CONFLICTED: return "conflicted";
        case UMI_VCS_ADVANCED_STATE_COMPLETED: return "completed";
        case UMI_VCS_ADVANCED_STATE_FAILED: return "failed";
        default: return "invalid";
    }
}

const char *umi_vcs_advanced_operation_text(UmiVcsAdvancedOperationKind kind)
{
    switch (kind) {
        case UMI_VCS_ADVANCED_OPERATION_NONE: return "none";
        case UMI_VCS_ADVANCED_OPERATION_STAGE: return "stage";
        case UMI_VCS_ADVANCED_OPERATION_UNSTAGE: return "unstage";
        case UMI_VCS_ADVANCED_OPERATION_BRANCH: return "branch";
        case UMI_VCS_ADVANCED_OPERATION_TAG: return "tag";
        case UMI_VCS_ADVANCED_OPERATION_STASH: return "stash";
        case UMI_VCS_ADVANCED_OPERATION_CHERRY_PICK: return "cherry-pick";
        case UMI_VCS_ADVANCED_OPERATION_REVERT: return "revert";
        case UMI_VCS_ADVANCED_OPERATION_RESET: return "reset";
        case UMI_VCS_ADVANCED_OPERATION_MERGE: return "merge";
        case UMI_VCS_ADVANCED_OPERATION_REBASE: return "rebase";
        case UMI_VCS_ADVANCED_OPERATION_PATCH: return "patch";
        default: return "invalid";
    }
}

const char *umi_vcs_compare_mode_text(UmiVcsCompareMode mode)
{
    switch (mode) {
        case UMI_VCS_COMPARE_TWO_WAY: return "two-way";
        case UMI_VCS_COMPARE_THREE_WAY: return "three-way";
        case UMI_VCS_COMPARE_DIRECTORY: return "directory";
        case UMI_VCS_COMPARE_SEMANTIC: return "semantic";
        case UMI_VCS_COMPARE_BINARY: return "binary";
        default: return "invalid";
    }
}

const char *umi_vcs_diff_kind_text(UmiVcsDiffKind kind)
{
    switch (kind) {
        case UMI_VCS_DIFF_CONTEXT: return "context";
        case UMI_VCS_DIFF_ADDED: return "added";
        case UMI_VCS_DIFF_DELETED: return "deleted";
        case UMI_VCS_DIFF_MODIFIED: return "modified";
        case UMI_VCS_DIFF_MOVED: return "moved";
        default: return "invalid";
    }
}

const char *umi_vcs_conflict_choice_text(UmiVcsConflictChoice choice)
{
    switch (choice) {
        case UMI_VCS_CONFLICT_UNRESOLVED: return "unresolved";
        case UMI_VCS_CONFLICT_USE_BASE: return "base";
        case UMI_VCS_CONFLICT_USE_OURS: return "ours";
        case UMI_VCS_CONFLICT_USE_THEIRS: return "theirs";
        case UMI_VCS_CONFLICT_USE_BOTH: return "both";
        case UMI_VCS_CONFLICT_MANUAL: return "manual";
        default: return "invalid";
    }
}
