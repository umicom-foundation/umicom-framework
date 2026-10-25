/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/whitespace.h
 *
 * PURPOSE:
 *   Prepare bounded whitespace transformations with mapped selections and immutable output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_WHITESPACE_H
#define UMICOM_EDITOR_WHITESPACE_H
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_WHITESPACE_TEXT_LIMIT (8U * 1024U * 1024U)
typedef enum UmiEditorWhitespaceOperation {
  UMI_EDITOR_WHITESPACE_TRIM_TRAILING = 1,
  UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS,
  UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES
} UmiEditorWhitespaceOperation;
typedef struct UmiEditorWhitespaceOptions {
  UmiEditorWhitespaceOperation operation;
  unsigned tab_width; /* 1..16; required even for trimming. */
  int selected_lines; /* 0: whole source; 1: complete lines touched by selection. */
} UmiEditorWhitespaceOptions;
typedef struct UmiEditorWhitespaceSummary {
  size_t source_bytes, proposed_bytes, touched_lines, changed_lines;
  size_t removed_bytes, inserted_bytes, cursor_offset, selection_bytes;
  int changed;
} UmiEditorWhitespaceSummary;
typedef struct UmiEditorWhitespacePlan UmiEditorWhitespacePlan;
/* Pure preparation: UTF-8 bytes, no NUL, at most eight MiB of input/output.
 * LF, CRLF and CR separators are retained exactly. Only ASCII space and tab
 * count as whitespace. Nonempty selected-line scope excludes a line whose
 * start is exactly the selection end; an empty selection means the caret line.
 * A final empty line is harmless and contains no leading/trailing whitespace.
 *
 * Conversion touches the leading run only and preserves its display columns
 * with tab stops measured from column zero. Compact uses tabs for complete
 * stops and spaces for the remainder; expand emits spaces. It does not parse
 * language syntax: tabs in Makefiles and trailing Markdown spaces can matter.
 * Present a review, never call this implicitly on Save.
 *
 * Cursor/selection endpoints are mapped to the same retained content; an
 * endpoint in converted indentation maps by display column (to the preceding
 * representable boundary in compacted tabs); one in removed trailing space
 * clamps to the trimmed end. Split UTF-8 and split CRLF endpoints fail.
 * Options are copied during the call. Cancellation/failure clears out_plan. */
UmiStatus UmiEditorWhitespacePlanCreate(const char *source, size_t bytes, size_t cursor_offset,
                                        size_t selection_bytes,
                                        const UmiEditorWhitespaceOptions *options,
                                        const UmiCancellationToken *cancel,
                                        UmiEditorWhitespacePlan **out_plan);
void UmiEditorWhitespacePlanDestroy(UmiEditorWhitespacePlan *plan);
UmiStatus UmiEditorWhitespacePlanInspect(const UmiEditorWhitespacePlan *plan,
                                         UmiEditorWhitespaceSummary *out_summary);
/* Borrow immutable proposed text until Destroy. Supplied outputs clear on error. */
UmiStatus UmiEditorWhitespacePlanRead(const UmiEditorWhitespacePlan *plan, const char **out_text,
                                      size_t *out_bytes);
#ifdef __cplusplus
}
#endif
#endif
