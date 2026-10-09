/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/source_navigation.h
 * PURPOSE: Select an exact protocol range through the live document and existing navigation history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SOURCE_NAVIGATION_H
#define UMICOM_DOCUMENT_SOURCE_NAVIGATION_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/text_position.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Find an already managed document by its exact captured view URI. This does
 * not open a file or activate a tab. Identity, not a display name, is returned.
 * Use on the coordinator's owning thread. Output changes only on success. */
    UmiStatus UmiDocumentCoordinatorFindSourceUri(const UmiDocumentCoordinator *coordinator, const char *uri,
                                                  UmiDocumentId *out_document);
    /* Resolve both zero-based UTF-16 endpoints against the complete current draft
 * before activating this document. Read-only drafts permit navigation.
 * Reject out-of-range, reversed or split-character positions without clamping.
 * Preserve source bytes, saved state and text Undo. A successful jump uses
 * the existing navigation history; selection length is measured in UTF-8 bytes.
 * Busy save/reopen/navigation operations are refused. No provider I/O occurs.
 * Optional outputs change only on success. Keep the coordinator and its
 * workbench alive throughout this owner-thread call. */
    UmiStatus UmiDocumentCoordinatorSelectSourceRange(UmiDocumentCoordinator *coordinator,
                                                      UmiDocumentId document, UmiEditorTextPosition start,
                                                      UmiEditorTextPosition end, size_t *out_offset,
                                                      size_t *out_bytes);
    /** Select a diagnostic or other captured range only while the named managed
     * document still has the same URI and complete UTF-8 source bytes. A moved
     * caret or changed active tab is allowed; a changed draft or renamed source
     * returns INVALID_STATE without navigation. No disk read, save or text edit
     * occurs. Read-only documents may be selected. This owner-thread call uses
     * the existing range resolver and navigation history, and preserves optional
     * outputs on failure. The caller keeps captured source alive through return. */
    UmiStatus UmiDocumentCoordinatorSelectCapturedSourceRange(
        UmiDocumentCoordinator *coordinator, UmiDocumentId document,
        const char *captured_uri, const char *captured_source, size_t source_bytes,
        UmiEditorTextPosition start, UmiEditorTextPosition end,
        size_t *out_offset, size_t *out_bytes);
    /* Resolve an already open view by URI or normalized local path. Otherwise,
 * decode a local file URI and load through the coordinator's existing provider.
 * Resolve the requested range before creating a new tab. Existing unsaved
 * drafts are never replaced with saved-file contents. URI queries, fragments,
 * remote authorities and implicit network paths are refused by local_uri.h.
 * This is synchronous provider I/O and must follow an explicit user action.
 *
 * One successful operation records one jump. Failure removes only a newly
 * attached target owned by this call and restores the prior active tab when
 * it still exists. No source text is written, saved or added to Undo.
 * Provider callbacks must keep the coordinator alive. Optional outputs change
 * only on success. A host should recheck its source request before calling. */
    UmiStatus UmiDocumentCoordinatorOpenSourceRange(UmiDocumentCoordinator *coordinator, const char *uri,
                                                    UmiEditorTextPosition start, UmiEditorTextPosition end,
                                                    UmiDocumentId *out_document, size_t *out_offset,
                                                    size_t *out_bytes);
#ifdef __cplusplus
}
#endif
#endif
