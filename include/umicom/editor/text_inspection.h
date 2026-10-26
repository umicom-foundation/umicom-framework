/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/text_inspection.h
 *
 * PURPOSE:
 *   Describe an owned text observation with explicit Unicode, line and invisible-character semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_EDITOR_TEXT_INSPECTION_H
#define UMICOM_EDITOR_TEXT_INSPECTION_H
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_INSPECTION_TEXT_LIMIT (8U * 1024U * 1024U)
#define UMI_EDITOR_INSPECTION_NOTICE_LIMIT 4096U
typedef struct UmiEditorTextInspection UmiEditorTextInspection;
typedef struct UmiEditorTextMetrics {
    size_t bytes, scalars, utf16_units, lines, words;
    size_t spaces, tabs, non_ascii, lf, crlf, cr;
    size_t blank_lines, trailing_whitespace_lines, longest_line_scalars;
    int ends_with_line_break;
} UmiEditorTextMetrics;
typedef enum UmiEditorTextNoticeKind {
    UMI_EDITOR_TEXT_BIDI_CONTROL = 1,
    UMI_EDITOR_TEXT_ZERO_WIDTH,
    UMI_EDITOR_TEXT_NONBREAKING_SPACE,
    UMI_EDITOR_TEXT_CONTROL
} UmiEditorTextNoticeKind;
typedef struct UmiEditorTextNotice {
    UmiEditorTextNoticeKind kind;
    uint32_t codepoint;
    size_t byte_offset, byte_length;
    size_t line, scalar_column; /* One-based; not visual or UTF-16 columns. */
} UmiEditorTextNotice;
typedef struct UmiEditorTextInspectionSummary {
    UmiEditorTextMetrics document, selection;
    size_t cursor_offset, selection_bytes;
    size_t notice_count, retained_notices;
    int notices_truncated;
} UmiEditorTextInspectionSummary;
/** Inspect an explicit UTF-8 span, at most 8 MiB, without I/O or mutations.
 * NUL and malformed UTF-8 are refused. Cursor/selection endpoints must be valid
 * scalar boundaries and cannot split CRLF. Input need not be terminated.
 * Malformed source returns PARSE_ERROR; after source validation, a split scalar
 * or CRLF endpoint returns INVALID_ARGUMENT. Out-of-range spans are rejected
 * before reading source bytes.
 * Empty text has one logical line; a terminal line break adds an empty line.
 * "Words" are runs separated by ASCII space, tab, CR, LF, vertical tab or form
 * feed; this is not natural-language segmentation. UTF-16 units include line
 * breaks. Blank/trailing-whitespace lines use ASCII space/tab only.
 * Notices describe selected invisible characters; they do not prove malicious
 * code and do not provide a complete Unicode-security analysis. The first 4096
 * notices are retained in byte order; totals always cover the complete input.
 * A zero-length selection is inspected as empty text, not as the whole document.
 * The returned immutable owner copies its text. It is independent of subsequent
 * edits or input lifetime. Serialize destruction against readers. A nullable
 * cancellation token is checked during scans. out must address NULL; it remains
 * unchanged on refusal. Keep input and output storage disjoint. */
UmiStatus UmiEditorTextInspectionCreate(const char *text, size_t bytes,
    size_t cursor, size_t selection, const UmiCancellationToken *cancel,
    UmiEditorTextInspection **out);
UmiStatus UmiEditorTextInspectionSummaryRead(const UmiEditorTextInspection *inspection,
    UmiEditorTextInspectionSummary *out);
UmiStatus UmiEditorTextInspectionNoticeAt(const UmiEditorTextInspection *inspection,
    size_t index, UmiEditorTextNotice *out);
/** Borrow exact terminated UTF-8 bytes until destruction; output pointers are
 * preserved on refusal. No report or raw source is published automatically. */
UmiStatus UmiEditorTextInspectionRead(const UmiEditorTextInspection *inspection,
    const char **text, size_t *bytes);
void UmiEditorTextInspectionDestroy(UmiEditorTextInspection *inspection);
#ifdef __cplusplus
}
#endif
#endif
