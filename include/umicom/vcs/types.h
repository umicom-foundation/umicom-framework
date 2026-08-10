/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/types.h
 *
 * PURPOSE:
 *   Define provider-neutral version-control states, limits and snapshots for Git and future providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
#define UMI_VCS_MAX_CHANGES 4096U
#define UMI_VCS_MAX_BRANCHES 512U
#define UMI_VCS_MAX_COMMITS 2048U

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

typedef struct UmiVcsChange {
    char path[UMI_VCS_PATH_CAPACITY];
    char original_path[UMI_VCS_PATH_CAPACITY];
    UmiVcsChangeState index_state;
    UmiVcsChangeState worktree_state;
    int staged;
} UmiVcsChange;

typedef struct UmiVcsBranch {
    char name[UMI_VCS_NAME_CAPACITY];
    char upstream[UMI_VCS_NAME_CAPACITY];
    int current;
    int detached;
    int ahead;
    int behind;
} UmiVcsBranch;

typedef struct UmiVcsCommit {
    char commit_id[UMI_VCS_ID_CAPACITY];
    char author[UMI_VCS_NAME_CAPACITY];
    char email[UMI_VCS_NAME_CAPACITY];
    char subject[UMI_VCS_MESSAGE_CAPACITY];
    int64_t timestamp;
} UmiVcsCommit;

typedef struct UmiVcsDiffStat {
    char path[UMI_VCS_PATH_CAPACITY];
    size_t insertions;
    size_t deletions;
    int binary;
} UmiVcsDiffStat;

const char *umi_vcs_change_state_text(UmiVcsChangeState state);

#ifdef __cplusplus
}
#endif

#endif
