/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/diff_navigation.c
 *
 * PURPOSE:
 *   Implement deterministic diff navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/diff_navigation.h"

/*
 * Provide the ai developer diff next change operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_next_change(
    const UmiAiDeveloperFileDiff *diff,
    size_t current_index,
    size_t *out_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || out_index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index =
        current_index < diff->line_count
            ? current_index + 1U
            : 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (; index < diff->line_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (diff->lines[index].kind != UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < diff->line_count && index <= current_index;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (diff->lines[index].kind != UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ai developer diff previous change operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_diff_previous_change(
    const UmiAiDeveloperFileDiff *diff,
    size_t current_index,
    size_t *out_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || out_index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index =
        current_index > diff->line_count
            ? diff->line_count
            : current_index;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        index -= 1U;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (diff->lines[index].kind != UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }

    index = diff->line_count;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > current_index + 1U) {
        index -= 1U;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (diff->lines[index].kind != UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ai developer diff hunk for line operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_hunk_for_line(
    const UmiAiDeveloperFileDiff *diff,
    size_t line_index,
    size_t *out_hunk_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || out_hunk_index == NULL ||
        line_index >= diff->line_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < diff->hunk_count; ++index) {
        const UmiAiDeveloperDiffHunk *hunk = &diff->hunks[index];

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (line_index >= hunk->first_line_index &&
            line_index < hunk->first_line_index + hunk->line_count) {
            *out_hunk_index = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
