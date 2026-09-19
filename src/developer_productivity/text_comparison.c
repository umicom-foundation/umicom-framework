/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/text_comparison.c
 *
 * PURPOSE:
 *   Retain complete comparison snapshots and reuse bounded line alignment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/text_comparison.h"
#include "umicom/developer_productivity/diff_navigation.h"

#include <stdlib.h>
#include <string.h>

struct UmiTextComparison {
    char *left;
    char *right;
    UmiDeveloperDiffDocument *alignment;
    UmiTextComparisonSummary summary;
};

/* Count physical text lines without counting a terminator as another line. */
static size_t CountLines(const char *text, size_t length)
{
    if (length == 0U) return 0U;
    size_t count = text[length - 1U] == '\n' ? 0U : 1U;
    for (size_t index = 0U; index < length; ++index)
        if (text[index] == '\n') ++count;
    return count;
}

UmiStatus UmiTextComparisonCreate(const char *left, size_t leftLength,
    const char *right, size_t rightLength, UmiTextComparison **outComparison)
{
    if (outComparison == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outComparison = NULL;
    if (left == NULL || right == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (leftLength > UMI_TEXT_COMPARISON_MAXIMUM_BYTES ||
        rightLength > UMI_TEXT_COMPARISON_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(left, '\0', leftLength) != NULL ||
        memchr(right, '\0', rightLength) != NULL ||
        left[leftLength] != '\0' || right[rightLength] != '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTextComparison *comparison = calloc(1U, sizeof *comparison);
    if (comparison == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    comparison->left = malloc(leftLength + 1U);
    comparison->right = malloc(rightLength + 1U);
    if (comparison->left == NULL || comparison->right == NULL) {
        UmiTextComparisonDestroy(comparison);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(comparison->left, left, leftLength + 1U);
    memcpy(comparison->right, right, rightLength + 1U);
    comparison->summary.leftBytes = leftLength;
    comparison->summary.rightBytes = rightLength;
    comparison->summary.leftLines = CountLines(left, leftLength);
    comparison->summary.rightLines = CountLines(right, rightLength);
    comparison->summary.identicalBytes = leftLength == rightLength &&
        memcmp(left, right, leftLength) == 0;
    comparison->summary.leftFinalNewline = leftLength != 0U && left[leftLength - 1U] == '\n';
    comparison->summary.rightFinalNewline = rightLength != 0U && right[rightLength - 1U] == '\n';
    UmiStatus status = umi_developer_diff_document_create(comparison->left,
        comparison->right, NULL, &comparison->alignment);
    comparison->summary.alignmentStatus = status;
    if (status != UMI_STATUS_OK && status != UMI_STATUS_CAPACITY_EXCEEDED) {
        UmiTextComparisonDestroy(comparison);
        return status;
    }
    if (status == UMI_STATUS_OK) {
        comparison->summary.rowCount = umi_developer_diff_document_row_count(comparison->alignment);
        comparison->summary.changes = umi_developer_diff_document_stats(comparison->alignment);
    }
    *outComparison = comparison;
    return UMI_STATUS_OK;
}

void UmiTextComparisonDestroy(UmiTextComparison *comparison)
{
    if (comparison == NULL) return;
    umi_developer_diff_document_destroy(comparison->alignment);
    free(comparison->right);
    free(comparison->left);
    free(comparison);
}

UmiStatus UmiTextComparisonGetSummary(const UmiTextComparison *comparison,
    UmiTextComparisonSummary *outSummary)
{
    if (comparison == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = comparison->summary;
    return UMI_STATUS_OK;
}

UmiStatus UmiTextComparisonText(const UmiTextComparison *comparison,
    int rightSide, const char **outText, size_t *outLength)
{
    if (outText != NULL) *outText = NULL;
    if (outLength != NULL) *outLength = 0U;
    if (comparison == NULL || outText == NULL || outLength == NULL ||
        (rightSide != 0 && rightSide != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    *outText = rightSide ? comparison->right : comparison->left;
    *outLength = rightSide ? comparison->summary.rightBytes : comparison->summary.leftBytes;
    return UMI_STATUS_OK;
}

UmiStatus UmiTextComparisonRow(const UmiTextComparison *comparison,
    size_t row, UmiDeveloperDiffRow *outRow)
{
    if (comparison == NULL || outRow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (comparison->summary.alignmentStatus != UMI_STATUS_OK)
        return comparison->summary.alignmentStatus;
    return umi_developer_diff_document_row_at(comparison->alignment, row, outRow);
}

UmiStatus UmiTextComparisonNavigate(const UmiTextComparison *comparison,
    size_t currentRow, int direction, size_t *outRow)
{
    if (comparison == NULL || outRow == NULL || (direction != 1 && direction != -1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (comparison->summary.alignmentStatus != UMI_STATUS_OK)
        return comparison->summary.alignmentStatus;
    return direction == 1
        ? umi_developer_diff_next_change(comparison->alignment, currentRow, outRow)
        : umi_developer_diff_previous_change(comparison->alignment, currentRow, outRow);
}
