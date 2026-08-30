/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/side_by_side.h
 *
 * PURPOSE:
 *   Project a unified line diff into paired left/right rows suitable for
 *   Beyond-Compare-style split review surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_SIDE_BY_SIDE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_SIDE_BY_SIDE_H
#include "umicom/ai_developer_experience/diff.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_SIDE_BY_SIDE_CAPACITY \
    UMI_AI_DEVELOPER_DIFF_LINE_CAPACITY

typedef enum UmiAiDeveloperSideBySideKind {
    UMI_AI_DEVELOPER_SIDE_EQUAL = 0,
    UMI_AI_DEVELOPER_SIDE_CHANGED = 1,
    UMI_AI_DEVELOPER_SIDE_LEFT_ONLY = 2,
    UMI_AI_DEVELOPER_SIDE_RIGHT_ONLY = 3
} UmiAiDeveloperSideBySideKind;

typedef struct UmiAiDeveloperSideBySideRow {
    UmiAiDeveloperSideBySideKind kind;
    uint32_t left_line;
    uint32_t right_line;
    char left_text[UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY];
    char right_text[UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY];
} UmiAiDeveloperSideBySideRow;

typedef struct UmiAiDeveloperSideBySideDiff {
    UmiAiDeveloperSideBySideRow rows[UMI_AI_DEVELOPER_SIDE_BY_SIDE_CAPACITY];
    size_t row_count;
    uint64_t revision;
    int truncated;
} UmiAiDeveloperSideBySideDiff;

UmiStatus umi_ai_developer_side_by_side_build(
    const UmiAiDeveloperFileDiff *diff,
    UmiAiDeveloperSideBySideDiff *out_split);

#ifdef __cplusplus
}
#endif
#endif
