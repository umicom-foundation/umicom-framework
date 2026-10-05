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
#ifdef __cplusplus
}
#endif
#endif
