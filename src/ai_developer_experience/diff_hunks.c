/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/diff_hunks.c
 *
 * PURPOSE:
 *   Implement hunk grouping around additions/removals while preserving bounded
 *   context rows for future IDE and web review surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/diff_hunks.h"

#include <string.h>

/* Provide the summarize hunk operation used by this module and its client applications. */
static void summarize_hunk(
    const UmiAiDeveloperFileDiff *diff,
    size_t first,
    size_t last,
    UmiAiDeveloperDiffHunk *hunk)
{
    size_t index;
    uint32_t old_start = 0U;
    uint32_t new_start = 0U;
    uint32_t old_count = 0U;
    uint32_t new_count = 0U;

    (void)memset(hunk, 0, sizeof(*hunk));
    hunk->first_line_index = first;
    hunk->line_count = last - first + 1U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = first; index <= last; ++index) {
        const UmiAiDeveloperDiffLine *line = &diff->lines[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (line->old_line != 0U) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (old_start == 0U) old_start = line->old_line;
            old_count += 1U;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (line->new_line != 0U) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (new_start == 0U) new_start = line->new_line;
            new_count += 1U;
        }
    }

    hunk->old_start = old_start;
    hunk->old_count = old_count;
    hunk->new_start = new_start;
    hunk->new_count = new_count;
}

/*
 * Provide the ai developer diff build hunks operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_build_hunks(
    UmiAiDeveloperFileDiff *diff,
    size_t context_lines)
{
    size_t index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    diff->hunk_count = 0U;
    (void)memset(diff->hunks, 0, sizeof(diff->hunks));

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < diff->line_count) {
        size_t change_index;
        size_t first;
        size_t last;
        size_t cursor;
        size_t trailing_context = 0U;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (index < diff->line_count &&
               diff->lines[index].kind == UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            index += 1U;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= diff->line_count) break;

        change_index = index;
        first =
            change_index > context_lines
                ? change_index - context_lines
                : 0U;
        last = change_index;

        cursor = change_index;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (cursor < diff->line_count) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (diff->lines[cursor].kind ==
                UMI_AI_DEVELOPER_DIFF_CONTEXT) {
                trailing_context += 1U;

                /* Apply this branch only when its contract condition is satisfied. */
                if (trailing_context > context_lines * 2U) {
                    last = cursor - trailing_context + context_lines;
                    break;
                }
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                trailing_context = 0U;
                last = cursor;
            }

            /* Apply this branch only when its contract condition is satisfied. */
            if (cursor + 1U == diff->line_count) {
                last = cursor;
                break;
            }

            cursor += 1U;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (diff->hunk_count >= UMI_AI_DEVELOPER_DIFF_HUNK_CAPACITY) {
            diff->truncated = 1;
            break;
        }

        summarize_hunk(
            diff,
            first,
            last,
            &diff->hunks[diff->hunk_count++]);

        index = last + 1U;
    }

    diff->revision += 1U;
    return UMI_STATUS_OK;
}
