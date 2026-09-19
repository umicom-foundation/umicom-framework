/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/text_comparison.h
 *
 * PURPOSE:
 *   Own two immutable texts for read-only review, reusing the existing bounded
 *   line-diff engine without treating its size limit as an editor text limit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TEXT_COMPARISON_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TEXT_COMPARISON_H

#include "umicom/developer_productivity/diff.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEXT_COMPARISON_MAXIMUM_BYTES (8U * 1024U * 1024U)

typedef struct UmiTextComparison UmiTextComparison;

typedef struct UmiTextComparisonSummary {
    size_t leftBytes;
    size_t rightBytes;
    size_t leftLines;
    size_t rightLines;
    size_t rowCount;
    UmiDeveloperDiffStats changes;
    UmiStatus alignmentStatus;
    int identicalBytes;
    int leftFinalNewline;
    int rightFinalNewline;
} UmiTextComparisonSummary;

/** Copy two terminated, NUL-free text spans. Lengths exclude the terminator.
 * Each span may contain up to 8 MiB. Empty input still requires a valid pointer.
 * This model accepts byte text; a graphical adapter separately requires UTF-8.
 * When the existing 512-line / line-length bound is exceeded, creation succeeds
 * with alignmentStatus = CAPACITY_EXCEEDED, retaining both complete texts.
 * Other diff failures are returned and no model is published. */
UmiStatus UmiTextComparisonCreate(const char *left, size_t leftLength,
    const char *right, size_t rightLength, UmiTextComparison **outComparison);

/** Release the model and its owned texts. Accepts NULL. */
void UmiTextComparisonDestroy(UmiTextComparison *comparison);

/** Copy immutable metadata. Byte identity is exact, independently of the diff
 * engine's line-ending normalisation. Zero changed rows does not establish
 * identical bytes (for example a changed final newline). */
UmiStatus UmiTextComparisonGetSummary(const UmiTextComparison *comparison,
    UmiTextComparisonSummary *outSummary);

/** Borrow one complete text until destruction. rightSide must be 0 or 1.
 * The returned text must not be modified/freed. Outputs are cleared on error. */
UmiStatus UmiTextComparisonText(const UmiTextComparison *comparison,
    int rightSide, const char **outText, size_t *outLength);

/** Copy an aligned row. Returns the alignment error for full-text-only models.
 * Row text excludes line terminators; line numbers are one based, or zero for
 * a missing side. Neither text is modified by review or navigation. */
UmiStatus UmiTextComparisonRow(const UmiTextComparison *comparison,
    size_t row, UmiDeveloperDiffRow *outRow);

/** Find a changed row in direction +1/-1. SIZE_MAX starts before the first row
 * when moving forward or after the last row when moving backwards. No wrap. */
UmiStatus UmiTextComparisonNavigate(const UmiTextComparison *comparison,
    size_t currentRow, int direction, size_t *outRow);

#ifdef __cplusplus
}
#endif
#endif
