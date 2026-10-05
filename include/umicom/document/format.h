/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/format.h
 * PURPOSE: Keep line-ending conversion and save encoding in the document-owned editing history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_FORMAT_H
#define UMICOM_DOCUMENT_FORMAT_H
#include "umicom/document/coordinator.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_FORMAT_TEXT_LIMIT (8U * 1024U * 1024U)
    typedef struct UmiDocumentFormatPlan UmiDocumentFormatPlan;
    typedef struct UmiDocumentFormatSummary
    {
        size_t source_bytes, proposed_bytes, cursor_offset, selection_bytes;
        size_t replaced_endings;
        int text_changes, added_final_newline;
    } UmiDocumentFormatSummary;
    typedef struct UmiDocumentFormatOptions
    {
        /* UNKNOWN retains the document's current save encoding. Other accepted
     * choices are UTF8, UTF8_BOM, UTF16_LE and UTF16_BE; none loses characters. */
        UmiDocumentTextEncoding encoding;
        /* NONE leaves source and the current save policy unchanged. LF, CRLF or
     * CR converts every existing ending and changes the next Save policy. */
        UmiDocumentLineEnding line_ending;
        int ensure_final_newline;
    } UmiDocumentFormatOptions;
    /* Build an independent UTF-8 proposal with exact selection mapping. CRLF is
 * one ending; endpoints inside it or a UTF-8 character are rejected. Non-ending
 * bytes remain identical. With NONE, ensure_final_newline must be zero.
 * Appending a final ending leaves an EOF caret before that new ending.
 * Input and output are limited to 8 MiB; embedded NUL is not source text.
 * Cancellation is checked between bounded phases. Failure clears out_plan. */
    UmiStatus UmiDocumentFormatPlanCreate(const char *source, size_t source_bytes, size_t cursor_offset,
                                          size_t selection_bytes, UmiDocumentLineEnding target,
                                          int ensure_final_newline, const UmiCancellationToken *cancel,
                                          UmiDocumentFormatPlan **out_plan);
    void UmiDocumentFormatPlanDestroy(UmiDocumentFormatPlan *plan);
    UmiStatus UmiDocumentFormatPlanInspect(const UmiDocumentFormatPlan *plan,
                                           UmiDocumentFormatSummary *out_summary);
    /* Borrow the complete proposed source until Destroy. Failure clears outputs. */
    UmiStatus UmiDocumentFormatPlanRead(const UmiDocumentFormatPlan *plan, const char **out_text,
                                        size_t *out_bytes);
    /* Apply to the active writable document on its owning thread. Prepare complete
 * text before changing source, save policy or history. Undo and Redo restore
 * both source selection and the previous save format. Even an encoding-only
 * change marks the document dirty and adds one history entry. Unchanged choices
 * do neither. Earlier pending typing keeps its own history step. No file is
 * written; use Save separately, with the usual external-change checks. */
    UmiStatus UmiDocumentCoordinatorSetFormat(UmiDocumentCoordinator *coordinator,
                                              const UmiDocumentFormatOptions *options);
#ifdef __cplusplus
}
#endif
#endif
