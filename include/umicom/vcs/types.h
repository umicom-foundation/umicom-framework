/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/types.h
 *
 * PURPOSE:
 *   Define provider-neutral version-control states, limits and snapshots for Git and future providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_TYPES_H
#define UMICOM_VCS_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_VCS_PATH_CAPACITY 2048U
#define UMI_VCS_NAME_CAPACITY 256U
#define UMI_VCS_ID_CAPACITY 128U
#define UMI_VCS_MESSAGE_CAPACITY 1024U
#define UMI_VCS_URL_CAPACITY 2048U
#define UMI_VCS_DIFF_TEXT_CAPACITY 4096U
#define UMI_VCS_MAX_CHANGES 4096U
#define UMI_VCS_MAX_BRANCHES 512U
#define UMI_VCS_MAX_COMMITS 2048U
#define UMI_VCS_MAX_REMOTES 128U
#define UMI_VCS_MAX_TAGS 2048U
#define UMI_VCS_MAX_DIFF_LINES 16384U
#define UMI_VCS_MAX_OPERATIONS 512U

/**
 * List the named vcs change state values accepted by this public contract.
 */
typedef enum UmiVcsChangeState {
    UMI_VCS_CHANGE_UNMODIFIED = 0,
    UMI_VCS_CHANGE_ADDED = 1,
    UMI_VCS_CHANGE_MODIFIED = 2,
    UMI_VCS_CHANGE_DELETED = 3,
    UMI_VCS_CHANGE_RENAMED = 4,
    UMI_VCS_CHANGE_COPIED = 5,
    UMI_VCS_CHANGE_UNTRACKED = 6,
    UMI_VCS_CHANGE_IGNORED = 7,
    UMI_VCS_CHANGE_CONFLICTED = 8
} UmiVcsChangeState;

/**
 * Represent the vcs change data shared with callers of this public contract.
 */
typedef struct UmiVcsChange {
    char path[UMI_VCS_PATH_CAPACITY];
    char original_path[UMI_VCS_PATH_CAPACITY];
    UmiVcsChangeState index_state;
    UmiVcsChangeState worktree_state;
    int staged;
} UmiVcsChange;

/**
 * Represent the vcs branch data shared with callers of this public contract.
 */
typedef struct UmiVcsBranch {
    char name[UMI_VCS_NAME_CAPACITY];
    char upstream[UMI_VCS_NAME_CAPACITY];
    int current;
    int detached;
    int ahead;
    int behind;
} UmiVcsBranch;

/**
 * Represent the vcs commit data shared with callers of this public contract.
 */
typedef struct UmiVcsCommit {
    char commit_id[UMI_VCS_ID_CAPACITY];
    char author[UMI_VCS_NAME_CAPACITY];
    char email[UMI_VCS_NAME_CAPACITY];
    char subject[UMI_VCS_MESSAGE_CAPACITY];
    int64_t timestamp;
} UmiVcsCommit;

/**
 * Represent the vcs diff stat data shared with callers of this public contract.
 */
typedef struct UmiVcsDiffStat {
    char path[UMI_VCS_PATH_CAPACITY];
    size_t insertions;
    size_t deletions;
    int binary;
} UmiVcsDiffStat;

/**
 * Represent the vcs remote data shared with callers of this public contract.
 */
typedef struct UmiVcsRemote {
    char name[UMI_VCS_NAME_CAPACITY];
    char fetch_url[UMI_VCS_URL_CAPACITY];
    char push_url[UMI_VCS_URL_CAPACITY];
} UmiVcsRemote;

/**
 * Represent the vcs tag data shared with callers of this public contract.
 */
typedef struct UmiVcsTag {
    char name[UMI_VCS_NAME_CAPACITY];
    char target_id[UMI_VCS_ID_CAPACITY];
    char subject[UMI_VCS_MESSAGE_CAPACITY];
} UmiVcsTag;

/**
 * List the named vcs conflict kind values accepted by this public contract.
 */
typedef enum UmiVcsConflictKind {
    UMI_VCS_CONFLICT_NONE = 0,
    UMI_VCS_CONFLICT_BOTH_ADDED = 1,
    UMI_VCS_CONFLICT_BOTH_MODIFIED = 2,
    UMI_VCS_CONFLICT_BOTH_DELETED = 3,
    UMI_VCS_CONFLICT_ADDED_BY_US = 4,
    UMI_VCS_CONFLICT_ADDED_BY_THEM = 5,
    UMI_VCS_CONFLICT_DELETED_BY_US = 6,
    UMI_VCS_CONFLICT_DELETED_BY_THEM = 7,
    UMI_VCS_CONFLICT_UNKNOWN = 8
} UmiVcsConflictKind;

/**
 * Represent the vcs conflict data shared with callers of this public contract.
 */
typedef struct UmiVcsConflict {
    char path[UMI_VCS_PATH_CAPACITY];
    UmiVcsConflictKind kind;
    char index_code[3];
} UmiVcsConflict;

/**
 * List the named vcs diff line kind values accepted by this public contract.
 */
typedef enum UmiVcsDiffLineKind {
    UMI_VCS_DIFF_CONTEXT = 0,
    UMI_VCS_DIFF_ADDITION = 1,
    UMI_VCS_DIFF_DELETION = 2,
    UMI_VCS_DIFF_HEADER = 3,
    UMI_VCS_DIFF_HUNK = 4,
    UMI_VCS_DIFF_NOTICE = 5
} UmiVcsDiffLineKind;

/**
 * Represent the vcs diff line data shared with callers of this public contract.
 */
typedef struct UmiVcsDiffLine {
    UmiVcsDiffLineKind kind;
    size_t old_line;
    size_t new_line;
    char text[UMI_VCS_DIFF_TEXT_CAPACITY];
} UmiVcsDiffLine;

/**
 * List the named vcs operation kind values accepted by this public contract.
 */
typedef enum UmiVcsOperationKind {
    UMI_VCS_OPERATION_REFRESH = 0,
    UMI_VCS_OPERATION_STAGE = 1,
    UMI_VCS_OPERATION_UNSTAGE = 2,
    UMI_VCS_OPERATION_DISCARD = 3,
    UMI_VCS_OPERATION_COMMIT = 4,
    UMI_VCS_OPERATION_FETCH = 5,
    UMI_VCS_OPERATION_PULL = 6,
    UMI_VCS_OPERATION_PUSH = 7,
    UMI_VCS_OPERATION_BRANCH_CREATE = 8,
    UMI_VCS_OPERATION_BRANCH_CHECKOUT = 9,
    UMI_VCS_OPERATION_BRANCH_DELETE = 10,
    UMI_VCS_OPERATION_DIFF = 11
} UmiVcsOperationKind;

/**
 * List the named vcs operation state values accepted by this public contract.
 */
typedef enum UmiVcsOperationState {
    UMI_VCS_OPERATION_PENDING = 0,
    UMI_VCS_OPERATION_RUNNING = 1,
    UMI_VCS_OPERATION_SUCCEEDED = 2,
    UMI_VCS_OPERATION_FAILED = 3,
    UMI_VCS_OPERATION_CANCELLED = 4
} UmiVcsOperationState;

/**
 * List the named vcs capability values accepted by this public contract.
 */
typedef enum UmiVcsCapability {
    UMI_VCS_CAPABILITY_STATUS = UINT64_C(1) << 0,
    UMI_VCS_CAPABILITY_HISTORY = UINT64_C(1) << 1,
    UMI_VCS_CAPABILITY_STAGE = UINT64_C(1) << 2,
    UMI_VCS_CAPABILITY_COMMIT = UINT64_C(1) << 3,
    UMI_VCS_CAPABILITY_PULL = UINT64_C(1) << 4,
    UMI_VCS_CAPABILITY_PUSH = UINT64_C(1) << 5,
    UMI_VCS_CAPABILITY_BRANCHES = UINT64_C(1) << 6,
    UMI_VCS_CAPABILITY_REMOTES = UINT64_C(1) << 7,
    UMI_VCS_CAPABILITY_TAGS = UINT64_C(1) << 8,
    UMI_VCS_CAPABILITY_DIFF = UINT64_C(1) << 9,
    UMI_VCS_CAPABILITY_STAGE_ALL = UINT64_C(1) << 10,
    UMI_VCS_CAPABILITY_DISCARD = UINT64_C(1) << 11,
    UMI_VCS_CAPABILITY_FETCH = UINT64_C(1) << 12,
    UMI_VCS_CAPABILITY_BRANCH_CREATE = UINT64_C(1) << 13,
    UMI_VCS_CAPABILITY_BRANCH_CHECKOUT = UINT64_C(1) << 14,
    UMI_VCS_CAPABILITY_BRANCH_DELETE = UINT64_C(1) << 15
} UmiVcsCapability;

/**
 * Represent the vcs operation data shared with callers of this public contract.
 */
typedef struct UmiVcsOperation {
    uint64_t operation_id;
    UmiVcsOperationKind kind;
    UmiVcsOperationState state;
    int status_code;
    int64_t started_at;
    int64_t finished_at;
    char subject[UMI_VCS_PATH_CAPACITY];
    char summary[UMI_VCS_MESSAGE_CAPACITY];
} UmiVcsOperation;

/**
 * Represent the vcs workspace snapshot data shared with callers of this public contract.
 */
typedef struct UmiVcsWorkspaceSnapshot {
    char root[UMI_VCS_PATH_CAPACITY];
    char provider_id[UMI_VCS_ID_CAPACITY];
    char branch[UMI_VCS_NAME_CAPACITY];
    char upstream[UMI_VCS_NAME_CAPACITY];
    int available;
    int detached;
    int ahead;
    int behind;
    size_t changes;
    size_t staged;
    size_t conflicts;
    size_t commits;
    size_t branches;
    size_t remotes;
    size_t tags;
    size_t diff_lines;
    size_t operations;
    uint64_t revision;
    uint64_t capabilities;
} UmiVcsWorkspaceSnapshot;

/**
 * Provide the vcs change state text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_change_state_text(UmiVcsChangeState state);
/**
 * Provide the vcs conflict kind text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_conflict_kind_text(UmiVcsConflictKind kind);
/**
 * Provide the vcs diff line kind text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_diff_line_kind_text(UmiVcsDiffLineKind kind);
/**
 * Provide the vcs operation kind text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_operation_kind_text(UmiVcsOperationKind kind);
/**
 * Provide the vcs operation state text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_operation_state_text(UmiVcsOperationState state);

#ifdef __cplusplus
}
#endif

#endif
