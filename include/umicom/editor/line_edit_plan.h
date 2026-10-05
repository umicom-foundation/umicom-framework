/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/line_edit_plan.h
 * PURPOSE: Prepare complete line-edit results without mutating an editor or creating another history owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_LINE_EDIT_PLAN_H
#define UMICOM_EDITOR_LINE_EDIT_PLAN_H
#include "umicom/editor/edit_command.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiEditorLineEditPlan UmiEditorLineEditPlan;
    typedef struct UmiEditorLineEditSummary
    {
        UmiEditorEditCommandKind kind;
        size_t source_bytes, proposed_bytes, cursor_offset, selection_bytes, affected_line_count;
        int changed;
    } UmiEditorLineEditSummary;
    /* Prepare DELETE_LINE, DUPLICATE_LINE, MOVE_LINE_UP/DOWN, JOIN_LINE_WITH_NEXT,
 * TRIM_TRAILING_WHITESPACE, INDENT_LINES, OUTDENT_LINES or TOGGLE_LINE_COMMENT.
 * Single-line commands use the caret line even when text is selected. Prefix
 * commands use all selected lines, excluding a line whose start is exactly
 * the selection end; an empty selection uses the caret line. Trim uses the
 * complete draft. These rules do not reinterpret language syntax.
 *
 * Source and endpoints must be valid UTF-8. LF and CRLF are supported; bare CR
 * returns NOT_IMPLEMENTED until the legacy line index supports that ending.
 * Source and proposed text are bounded to 8 MiB. Growth is checked before
 * transformation, so a conservative upper bound can reject a near-limit
 * comment toggle even when it would remove prefixes. Indentation defaults to
 * four spaces and accepts at most 16 spaces/tabs; comments default to // and
 * accept at most 32 printable non-space ASCII bytes. Both are literal tokens.
 *
 * Selected prefix edits retain a complete-line selection for repeated use.
 * An unselected prefix edit places the caret at the line start. Trim places
 * it at the document start. Unchanged text preserves the original selection.
 * Other caret positions follow the existing edit engine's transformation.
 * Inputs are borrowed until return; the successful plan owns complete output.
 * Cancellation is checked between bounded phases, not inside each transform.
 * Failure clears out_plan. No document, file, revision or history is changed. */
    UmiStatus UmiEditorLineEditPlanCreate(const char *source, size_t source_bytes,
                                          const UmiEditorEditCommandRequest *request,
                                          const UmiCancellationToken *cancel,
                                          UmiEditorLineEditPlan **out_plan);
    void UmiEditorLineEditPlanDestroy(UmiEditorLineEditPlan *plan);
    UmiStatus UmiEditorLineEditPlanInspect(const UmiEditorLineEditPlan *plan,
                                           UmiEditorLineEditSummary *out_summary);
    /* Borrow complete output until Destroy. Failure clears both outputs. */
    UmiStatus UmiEditorLineEditPlanRead(const UmiEditorLineEditPlan *plan, const char **out_text,
                                        size_t *out_bytes);
#ifdef __cplusplus
}
#endif
#endif
