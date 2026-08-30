/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/types.h
 *
 * PURPOSE:
 *   Define stable advanced source-control and comparison vocabulary.
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

#ifndef UMICOM_VCS_ADVANCED_TYPES_H
#define UMICOM_VCS_ADVANCED_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_VCS_ADVANCED_API_VERSION 1U
#define UMI_VCS_ADVANCED_ID_CAPACITY 96U
#define UMI_VCS_ADVANCED_OID_CAPACITY 72U
#define UMI_VCS_ADVANCED_PATH_CAPACITY 1024U
#define UMI_VCS_ADVANCED_TEXT_CAPACITY 512U
#define UMI_VCS_ADVANCED_LABEL_CAPACITY 128U
#define UMI_VCS_ADVANCED_SMALL_CAPACITY 64U
#define UMI_VCS_ADVANCED_LIST_CAPACITY 128U
#define UMI_VCS_ADVANCED_GRAPH_EDGE_CAPACITY 256U
#define UMI_VCS_ADVANCED_PATCH_CAPACITY 16384U
#define UMI_VCS_ADVANCED_MERGE_TEXT_CAPACITY 8192U

typedef enum UmiVcsAdvancedState {
    UMI_VCS_ADVANCED_STATE_UNKNOWN = 0,
    UMI_VCS_ADVANCED_STATE_READY = 1,
    UMI_VCS_ADVANCED_STATE_PENDING = 2,
    UMI_VCS_ADVANCED_STATE_BLOCKED = 3,
    UMI_VCS_ADVANCED_STATE_CONFLICTED = 4,
    UMI_VCS_ADVANCED_STATE_COMPLETED = 5,
    UMI_VCS_ADVANCED_STATE_FAILED = 6
} UmiVcsAdvancedState;

typedef enum UmiVcsAdvancedOperationKind {
    UMI_VCS_ADVANCED_OPERATION_NONE = 0,
    UMI_VCS_ADVANCED_OPERATION_STAGE = 1,
    UMI_VCS_ADVANCED_OPERATION_UNSTAGE = 2,
    UMI_VCS_ADVANCED_OPERATION_BRANCH = 3,
    UMI_VCS_ADVANCED_OPERATION_TAG = 4,
    UMI_VCS_ADVANCED_OPERATION_STASH = 5,
    UMI_VCS_ADVANCED_OPERATION_CHERRY_PICK = 6,
    UMI_VCS_ADVANCED_OPERATION_REVERT = 7,
    UMI_VCS_ADVANCED_OPERATION_RESET = 8,
    UMI_VCS_ADVANCED_OPERATION_MERGE = 9,
    UMI_VCS_ADVANCED_OPERATION_REBASE = 10,
    UMI_VCS_ADVANCED_OPERATION_PATCH = 11
} UmiVcsAdvancedOperationKind;

typedef enum UmiVcsCompareMode {
    UMI_VCS_COMPARE_TWO_WAY = 0,
    UMI_VCS_COMPARE_THREE_WAY = 1,
    UMI_VCS_COMPARE_DIRECTORY = 2,
    UMI_VCS_COMPARE_SEMANTIC = 3,
    UMI_VCS_COMPARE_BINARY = 4
} UmiVcsCompareMode;

typedef enum UmiVcsDiffKind {
    UMI_VCS_DIFF_CONTEXT = 0,
    UMI_VCS_DIFF_ADDED = 1,
    UMI_VCS_DIFF_DELETED = 2,
    UMI_VCS_DIFF_MODIFIED = 3,
    UMI_VCS_DIFF_MOVED = 4
} UmiVcsDiffKind;

typedef enum UmiVcsConflictChoice {
    UMI_VCS_CONFLICT_UNRESOLVED = 0,
    UMI_VCS_CONFLICT_USE_BASE = 1,
    UMI_VCS_CONFLICT_USE_OURS = 2,
    UMI_VCS_CONFLICT_USE_THEIRS = 3,
    UMI_VCS_CONFLICT_USE_BOTH = 4,
    UMI_VCS_CONFLICT_MANUAL = 5
} UmiVcsConflictChoice;

typedef enum UmiVcsWhitespaceMode {
    UMI_VCS_WHITESPACE_EXACT = 0,
    UMI_VCS_WHITESPACE_IGNORE_TRAILING = 1,
    UMI_VCS_WHITESPACE_IGNORE_AMOUNT = 2,
    UMI_VCS_WHITESPACE_IGNORE_ALL = 3
} UmiVcsWhitespaceMode;

typedef enum UmiVcsSafetyLevel {
    UMI_VCS_SAFETY_SAFE = 0,
    UMI_VCS_SAFETY_REVIEW = 1,
    UMI_VCS_SAFETY_DESTRUCTIVE = 2
} UmiVcsSafetyLevel;

typedef enum UmiVcsDirectoryState {
    UMI_VCS_DIRECTORY_EQUAL = 0,
    UMI_VCS_DIRECTORY_LEFT_ONLY = 1,
    UMI_VCS_DIRECTORY_RIGHT_ONLY = 2,
    UMI_VCS_DIRECTORY_DIFFERENT = 3,
    UMI_VCS_DIRECTORY_TYPE_CHANGED = 4
} UmiVcsDirectoryState;

UmiStatus umi_vcs_advanced_copy_text(char *destination,
                                     size_t capacity,
                                     const char *source);
uint64_t umi_vcs_advanced_hash_text(const char *text);
int umi_vcs_advanced_text_present(const char *text);
int umi_vcs_advanced_text_equal(const char *left, const char *right);
const char *umi_vcs_advanced_state_text(UmiVcsAdvancedState state);
const char *umi_vcs_advanced_operation_text(UmiVcsAdvancedOperationKind kind);
const char *umi_vcs_compare_mode_text(UmiVcsCompareMode mode);
const char *umi_vcs_diff_kind_text(UmiVcsDiffKind kind);
const char *umi_vcs_conflict_choice_text(UmiVcsConflictChoice choice);

#ifdef __cplusplus
}
#endif

#endif
