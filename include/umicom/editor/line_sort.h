/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/line_sort.h
 *
 * PURPOSE:
 *   Prepare stable selected-line ordering without mutating source owners or editor history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_LINE_SORT_H
#define UMICOM_EDITOR_LINE_SORT_H
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_LINE_SORT_TEXT_LIMIT (8U * 1024U * 1024U)
#define UMI_EDITOR_LINE_SORT_LINE_LIMIT 65536U
typedef struct UmiEditorLineSortPlan UmiEditorLineSortPlan;
typedef struct UmiEditorLineSortSummary {
  size_t source_bytes;
  size_t selection_start, selection_bytes, line_count;
  int changed;
} UmiEditorLineSortSummary;
/** Sort the complete lines touched by a nonempty UTF-8 byte selection. Endpoints
 * may be reversed; a selection ending exactly at a line start excludes that
 * line. Empty selections return NOT_FOUND, never a whole-document sort.
 * Order is ordinal UTF-8 byte order, with no locale, numeric or case folding.
 * Equal lines retain their original order. descending must be 0 or 1.
 *
 * Content moves while each output position keeps its original LF/CRLF separator.
 * The final newline remains present or absent. Bare CR returns NOT_IMPLEMENTED.
 * Text outside the expanded line selection is unchanged. A changed result selects
 * the complete reordered block; an unchanged result keeps the input selection.
 *
 * Input is borrowed during this call; a successful plan owns a complete output.
 * No provider, file, document or Undo state is changed. Cancellation is checked
 * between scans, merge groups and output lines. Inputs larger than eight MiB or
 * selections touching more than 65536 lines fail before unbounded allocation.
 * Failure clears outPlan. Caller serializes calls and keeps the plan alive. */
UmiStatus UmiEditorLineSortPlanCreate(const char *source, size_t bytes, size_t selectionStart,
                                      size_t selectionEnd, int descending,
                                      const UmiCancellationToken *cancel,
                                      UmiEditorLineSortPlan **outPlan);
UmiStatus UmiEditorLineSortPlanInspect(const UmiEditorLineSortPlan *plan,
                                       UmiEditorLineSortSummary *outSummary);
UmiStatus UmiEditorLineSortPlanRead(const UmiEditorLineSortPlan *plan, const char **outText,
                                    size_t *outBytes);
void UmiEditorLineSortPlanDestroy(UmiEditorLineSortPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
