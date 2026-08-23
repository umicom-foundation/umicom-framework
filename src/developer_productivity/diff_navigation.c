/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diff_navigation.c
 *
 * PURPOSE:
 *   Implement deterministic compare navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diff_navigation.h"

UmiStatus umi_developer_diff_next_change(
    const UmiDeveloperDiffDocument *document,
    size_t after_row,
    size_t *out_row)
{
    size_t index;
    const size_t count =
        umi_developer_diff_document_row_count(document);

    if (document == NULL || out_row == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = after_row + 1U; index < count; ++index) {
        UmiDeveloperDiffRow row;
        if (umi_developer_diff_document_row_at(
                document, index, &row) == UMI_STATUS_OK &&
            row.kind != UMI_DEVELOPER_DIFF_EQUAL) {
            *out_row = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_developer_diff_previous_change(
    const UmiDeveloperDiffDocument *document,
    size_t before_row,
    size_t *out_row)
{
    size_t index;

    if (document == NULL || out_row == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = before_row;
    while (index > 0U) {
        UmiDeveloperDiffRow row;
        index -= 1U;

        if (umi_developer_diff_document_row_at(
                document, index, &row) == UMI_STATUS_OK &&
            row.kind != UMI_DEVELOPER_DIFF_EQUAL) {
            *out_row = index;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
