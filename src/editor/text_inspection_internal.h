/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_inspection_internal.h
 *
 * PURPOSE:
 *   Share bounded scanning state while keeping inspection allocation and Unicode classification
 * private.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEXT_INSPECTION_INTERNAL_H
#define UMICOM_TEXT_INSPECTION_INTERNAL_H
#include "umicom/editor/text_inspection.h"
struct UmiEditorTextInspection {
  char *text;
  UmiEditorTextInspectionSummary summary;
  UmiEditorTextNotice *notices;
};
/* Decode one scalar and reject truncated, overlong, surrogate and out-of-range
 * encodings. No caller may assume the input is NUL-terminated. */
UmiStatus InspectionScalar(const char *text, size_t bytes, size_t offset, uint32_t *scalar,
                           size_t *width);
UmiEditorTextNoticeKind InspectionNoticeKind(uint32_t scalar);
UmiStatus InspectionMeasure(const char *text, size_t bytes, const UmiCancellationToken *cancel,
                            UmiEditorTextMetrics *metrics, UmiEditorTextInspection *notices);
#endif
