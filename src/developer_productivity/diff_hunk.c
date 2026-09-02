/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diff_hunk.c
 *
 * PURPOSE:
 *   Build navigable hunks around changed side-by-side rows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diff_hunk.h"

#include <string.h>

/*
 * Provide the developer diff hunks build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_diff_hunks_build(
    const UmiDeveloperDiffDocument *document,
    size_t context_lines,
    UmiDeveloperDiffHunkList *out_hunks)
{
    size_t row_count;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_hunks == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_hunks, 0, sizeof(*out_hunks));
    row_count = umi_developer_diff_document_row_count(document);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < row_count; ++index) {
        UmiDeveloperDiffRow row;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_developer_diff_document_row_at(
                document, index, &row) != UMI_STATUS_OK) {
            return UMI_STATUS_INTERNAL_ERROR;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (row.kind == UMI_DEVELOPER_DIFF_EQUAL) continue;

        {
            size_t first = index > context_lines
                ? index - context_lines
                : 0U;
            size_t last =
                index + context_lines < row_count
                    ? index + context_lines
                    : row_count - 1U;
            UmiDeveloperDiffHunk *hunk;

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_hunks->count > 0U &&
                first <= out_hunks->hunks[out_hunks->count - 1U].last_row + 1U) {
                hunk = &out_hunks->hunks[out_hunks->count - 1U];
                /* Apply this branch only when its contract condition is satisfied. */
                if (last > hunk->last_row) hunk->last_row = last;
                hunk->changed_rows += 1U;
                continue;
            }

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_hunks->count >= UMI_DEVELOPER_DIFF_MAX_HUNKS) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            hunk = &out_hunks->hunks[out_hunks->count++];
            hunk->first_row = first;
            hunk->last_row = last;
            hunk->changed_rows = 1U;
            hunk->left_start = row.left_line;
            hunk->right_start = row.right_line;
        }
    }

    return UMI_STATUS_OK;
}
