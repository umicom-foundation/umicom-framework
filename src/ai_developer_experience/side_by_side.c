/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/side_by_side.c
 *
 * PURPOSE:
 *   Build paired review rows while keeping the canonical unified diff intact.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/side_by_side.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *out, size_t capacity, const char *text)
{
    size_t length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(out, text, length);
    out[length] = '\0';
}

/*
 * Provide the ai developer side by side build operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_side_by_side_build(
    const UmiAiDeveloperFileDiff *diff,
    UmiAiDeveloperSideBySideDiff *out_split)
{
    size_t index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || out_split == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_split, 0, sizeof(*out_split));
    out_split->revision = diff->revision;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < diff->line_count) {
        UmiAiDeveloperSideBySideRow *row;
        const UmiAiDeveloperDiffLine *line = &diff->lines[index];

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_split->row_count >=
            UMI_AI_DEVELOPER_SIDE_BY_SIDE_CAPACITY) {
            out_split->truncated = 1;
            break;
        }

        row = &out_split->rows[out_split->row_count++];
        (void)memset(row, 0, sizeof(*row));

        /* Apply this branch only when its contract condition is satisfied. */
        if (line->kind == UMI_AI_DEVELOPER_DIFF_CONTEXT) {
            row->kind = UMI_AI_DEVELOPER_SIDE_EQUAL;
            row->left_line = line->old_line;
            row->right_line = line->new_line;
            copy_text(row->left_text, sizeof(row->left_text), line->text);
            copy_text(row->right_text, sizeof(row->right_text), line->text);
            index += 1U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (line->kind == UMI_AI_DEVELOPER_DIFF_REMOVE &&
            index + 1U < diff->line_count &&
            diff->lines[index + 1U].kind == UMI_AI_DEVELOPER_DIFF_ADD) {
            const UmiAiDeveloperDiffLine *added =
                &diff->lines[index + 1U];

            row->kind = UMI_AI_DEVELOPER_SIDE_CHANGED;
            row->left_line = line->old_line;
            row->right_line = added->new_line;
            copy_text(row->left_text, sizeof(row->left_text), line->text);
            copy_text(row->right_text, sizeof(row->right_text), added->text);
            index += 2U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (line->kind == UMI_AI_DEVELOPER_DIFF_ADD &&
            index + 1U < diff->line_count &&
            diff->lines[index + 1U].kind == UMI_AI_DEVELOPER_DIFF_REMOVE) {
            const UmiAiDeveloperDiffLine *removed =
                &diff->lines[index + 1U];

            row->kind = UMI_AI_DEVELOPER_SIDE_CHANGED;
            row->left_line = removed->old_line;
            row->right_line = line->new_line;
            copy_text(row->left_text, sizeof(row->left_text), removed->text);
            copy_text(row->right_text, sizeof(row->right_text), line->text);
            index += 2U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (line->kind == UMI_AI_DEVELOPER_DIFF_REMOVE) {
            row->kind = UMI_AI_DEVELOPER_SIDE_LEFT_ONLY;
            row->left_line = line->old_line;
            copy_text(row->left_text, sizeof(row->left_text), line->text);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            row->kind = UMI_AI_DEVELOPER_SIDE_RIGHT_ONLY;
            row->right_line = line->new_line;
            copy_text(row->right_text, sizeof(row->right_text), line->text);
        }

        index += 1U;
    }

    return UMI_STATUS_OK;
}
