/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/diff_navigation.h
 *
 * PURPOSE:
 *   Provide next/previous change and hunk navigation for toolkit-neutral review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_NAVIGATION_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_NAVIGATION_H
#include "umicom/ai_developer_experience/diff.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_diff_next_change(
    const UmiAiDeveloperFileDiff *diff,
    size_t current_index,
    size_t *out_index);

UmiStatus umi_ai_developer_diff_previous_change(
    const UmiAiDeveloperFileDiff *diff,
    size_t current_index,
    size_t *out_index);

UmiStatus umi_ai_developer_diff_hunk_for_line(
    const UmiAiDeveloperFileDiff *diff,
    size_t line_index,
    size_t *out_hunk_index);

#ifdef __cplusplus
}
#endif
#endif
