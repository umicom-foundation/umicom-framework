/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diff_navigation.c
 *
 * PURPOSE:
 *   Implement deterministic compare navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diff_navigation.h"

/*
 * Provide the developer diff next change operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_diff_next_change(
    const UmiDeveloperDiffDocument *document,
    size_t after_row,
    size_t *out_row)
{
    size_t index;
    const size_t count =
        umi_developer_diff_document_row_count(document);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_row == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = after_row + 1U; index < count; ++index) {
        UmiDeveloperDiffRow row;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_developer_diff_document_row_at(
                document, index, &row) == UMI_STATUS_OK &&
            row.kind != UMI_DEVELOPER_DIFF_EQUAL) {
            *out_row = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the developer diff previous change operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_diff_previous_change(
    const UmiDeveloperDiffDocument *document,
    size_t before_row,
    size_t *out_row)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_row == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = before_row;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        UmiDeveloperDiffRow row;
        index -= 1U;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_developer_diff_document_row_at(
                document, index, &row) == UMI_STATUS_OK &&
            row.kind != UMI_DEVELOPER_DIFF_EQUAL) {
            *out_row = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
