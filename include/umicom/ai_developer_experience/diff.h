/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/diff.h
 *
 * PURPOSE:
 *   Define a bounded line-oriented diff model reusable by AI patch review,
 *   source-control review and future Beyond-Compare-style IDE surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY 1024U

/**
 * Represent the ai developer diff line data shared with callers of this public contract.
 */
typedef struct UmiAiDeveloperDiffLine {
    UmiAiDeveloperDiffLineKind kind;
    uint32_t old_line;
    uint32_t new_line;
    char text[UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY];
} UmiAiDeveloperDiffLine;

/**
 * Represent the ai developer diff hunk data shared with callers of this public contract.
 */
typedef struct UmiAiDeveloperDiffHunk {
    size_t first_line_index;
    size_t line_count;
    uint32_t old_start;
    uint32_t old_count;
    uint32_t new_start;
    uint32_t new_count;
} UmiAiDeveloperDiffHunk;

/**
 * Represent the ai developer file diff data shared with callers of this public contract.
 */
typedef struct UmiAiDeveloperFileDiff {
    char path[UMI_AI_DEVELOPER_PATH_CAPACITY];
    UmiAiCodingPatchOperation operation;
    UmiAiDeveloperDiffLine lines[UMI_AI_DEVELOPER_DIFF_LINE_CAPACITY];
    UmiAiDeveloperDiffHunk hunks[UMI_AI_DEVELOPER_DIFF_HUNK_CAPACITY];
    size_t line_count;
    size_t hunk_count;
    size_t added_count;
    size_t removed_count;
    size_t context_count;
    uint64_t before_hash;
    uint64_t after_hash;
    uint64_t revision;
    int truncated;
} UmiAiDeveloperFileDiff;

#ifdef __cplusplus
}
#endif
#endif
