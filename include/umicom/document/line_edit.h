/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/line_edit.h
 * PURPOSE: Apply shared line transformations through the document coordinator and its existing Undo history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_LINE_EDIT_H
#define UMICOM_DOCUMENT_LINE_EDIT_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/line_edit_plan.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* The initial NULL-options behavior remains four spaces. Applications may
     * now change that default through document/indentation.h. Supplying an
     * options object retains its literal-token behavior, including its legacy
     * four-space fallback for NULL indent_text. No global setting is read. */
    typedef struct UmiDocumentLineEditOptions
    {
        const char *indent_text;
        const char *line_comment;
    } UmiDocumentLineEditOptions;
    /* Edit the active draft using the operation rules in line_edit_plan.h. The
 * current view supplies the exact caret and selection; callers cannot redirect
 * a toolbar command to a stale position. NULL options use four-space indent
 * and the literal // comment prefix. These tokens are not language detection.
 * Options are borrowed only until return. Use the coordinator owner thread.
 *
 * Build the whole proposal before committing through the existing document
 * history. Unsynchronized earlier typing retains its own Undo step; this edit
 * adds one step only if text changes. Read-only drafts and busy owners refuse
 * the operation. No file save, provider request or second history owner occurs. */
    UmiStatus UmiDocumentCoordinatorEditLines(UmiDocumentCoordinator *coordinator,
                                              UmiEditorEditCommandKind kind,
                                              const UmiDocumentLineEditOptions *options);
    /** Edit the specified open document without changing the active tab. The
     * selected document owns its caret, text and Undo step. A closed or unknown
     * identity returns NOT_FOUND; zero is INVALID_ARGUMENT and never means the
     * current tab. All other transaction and ownership rules above still apply. */
    UmiStatus UmiDocumentCoordinatorEditLinesForDocument(UmiDocumentCoordinator *coordinator,
        UmiDocumentId document_id, UmiEditorEditCommandKind kind,
        const UmiDocumentLineEditOptions *options);
    /** Inspect current line-edit admission without changing text, selection,
     * active tab, revision or history. Use the coordinator owner thread. Zero
     * identity, NULL owner and unsupported kinds are INVALID_ARGUMENT; a closed
     * identity or an empty selection for case/sort is NOT_FOUND. A read-only
     * draft is PERMISSION_DENIED and an occupied owner is BUSY.
     *
     * OK covers identity, bounds, editability and required selection only. It
     * does not validate neighboring lines, encoding, tokens or capacity, reserve
     * an edit, or authorize later mutation. Execute through EditLinesForDocument
     * and handle its result even after this query succeeds. */
    UmiStatus UmiDocumentCoordinatorCheckLineEdit(UmiDocumentCoordinator *coordinator,
        UmiDocumentId document_id, UmiEditorEditCommandKind kind);
#ifdef __cplusplus
}
#endif
#endif
