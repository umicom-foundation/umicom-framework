/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/diff_hunks.c
 *
 * PURPOSE:
 *   Implement hunk grouping around additions/removals while preserving bounded
 *   context rows for future IDE and web review surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/diff_hunks.h"

#include <string.h>

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

    for (index = first; index <= last; ++index) {
        const UmiAiDeveloperDiffLine *line = &diff->lines[index];

        if (line->old_line != 0U) {
            if (old_start == 0U) old_start = line->old_line;
            old_count += 1U;
        }

        if (line->new_line != 0U) {
            if (new_start == 0U) new_start = line->new_line;
            new_count += 1U;
        }
    }

    hunk->old_start = old_start;
    hunk->old_count = old_count;
    hunk->new_start = new_start;
    hunk->new_count = new_count;
}

UmiStatus umi_ai_developer_diff_build_hunks(
    UmiAiDeveloperFileDiff *diff,
    size_t context_lines)
{
    size_t index = 0U;

    if (diff == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    diff->hunk_count = 0U;
    (void)memset(diff->hunks, 0, sizeof(diff->hunks));

    while (index < diff->line_count) {
        size_t change_index;
        size_t first;
        size_t last;
        size_t cursor;
        size_t trailing_context = 0U;

        while (index < diff->line_count &&
               diff->lines[index].kind == UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            index += 1U;
        }

        if (index >= diff->line_count) break;

        change_index = index;
        first =
            change_index > context_lines
                ? change_index - context_lines
                : 0U;
        last = change_index;

        cursor = change_index;

        while (cursor < diff->line_count) {
            if (diff->lines[cursor].kind ==
                UMI_AI_DEVELOPER_DIFF_CONTEXT) {
                trailing_context += 1U;

                if (trailing_context > context_lines * 2U) {
                    last = cursor - trailing_context + context_lines;
                    break;
                }
            } else {
                trailing_context = 0U;
                last = cursor;
            }

            if (cursor + 1U == diff->line_count) {
                last = cursor;
                break;
            }

            cursor += 1U;
        }

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
