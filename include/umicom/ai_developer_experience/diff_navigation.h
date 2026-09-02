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

/**
 * Provide the ai developer diff next change operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_next_change(
    const UmiAiDeveloperFileDiff *diff,
    size_t current_index,
    size_t *out_index);

/**
 * Provide the ai developer diff previous change operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_diff_previous_change(
    const UmiAiDeveloperFileDiff *diff,
    size_t current_index,
    size_t *out_index);

/**
 * Provide the ai developer diff hunk for line operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_hunk_for_line(
    const UmiAiDeveloperFileDiff *diff,
    size_t line_index,
    size_t *out_hunk_index);

#ifdef __cplusplus
}
#endif
#endif
