/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/replacement_set.h
 * PURPOSE: Review all open draft replacements before applying one complete document transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_REPLACEMENT_SET_H
#define UMICOM_DOCUMENT_REPLACEMENT_SET_H
#include "umicom/document/source_batch.h"
#include "umicom/editor/search_engine.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDocumentReplacementSet UmiDocumentReplacementSet;
    typedef struct UmiDocumentReplacementSetSummary
    {
        size_t document_count, changed_count, reviewed_count, match_count;
        uint64_t revision;
        int applied;
    } UmiDocumentReplacementSetSummary;

    /* Capture all currently open documents and prepare smart-case literal
 * replacements. Pinned and untitled documents are included. New tabs opened
 * later are excluded. Empty needles fail; empty replacements remove matches.
 * This complete-set workflow requires every captured document to be writable;
 * a read-only, invalid, oversized or inaccessible member rejects the capture.
 * The existing sequential replacement session remains available when individual
 * documents must be skipped. No source changes or disk I/O occur here.
 * Retain the coordinator and its services; use their common owner thread. */
    UmiStatus UmiDocumentReplacementSetCreate(UmiDocumentCoordinator *coordinator, const char *needle,
                                              const char *replacement, UmiDocumentReplacementSet **out_set);
    /** Capture explicit case and whole-word policy for every reviewed draft.
 * NULL retains smart-case defaults. Later changes to caller options cannot
 * alter this capture; complete replacement ignores match-count/overlap limits. */
UmiStatus UmiDocumentReplacementSetCreateWithOptions(UmiDocumentCoordinator *coordinator, const char *needle,
    const char *replacement, const UmiEditorSearchOptions *options, UmiDocumentReplacementSet **out_set);
void UmiDocumentReplacementSetDestroy(UmiDocumentReplacementSet *set);
    UmiStatus UmiDocumentReplacementSetInspect(const UmiDocumentReplacementSet *set,
                                               UmiDocumentReplacementSetSummary *out_summary);
    UmiStatus UmiDocumentReplacementSetAt(const UmiDocumentReplacementSet *set, size_t index,
                                          UmiDocumentSourceRequestSummary *out_document, size_t *out_matches,
                                          int *out_reviewed);
    /* Borrow full immutable before/after buffers. Destroy invalidates both;
 * successful Apply invalidates access to proposals. Copy for longer-lived UI. */
    UmiStatus UmiDocumentReplacementSetTexts(const UmiDocumentReplacementSet *set, size_t index,
                                             const char **out_previous, size_t *out_previous_bytes,
                                             const char **out_proposed, size_t *out_proposed_bytes);
    /* Mark a changed document reviewed only after presenting its complete comparison.
 * Review itself does not edit. Repeated review is harmless. Any stale member
 * rejects further review and Apply, including a changed document not yet shown.
 * Passing a different revision returns BUSY. Review flags do not advance it. */
    UmiStatus UmiDocumentReplacementSetReview(UmiDocumentReplacementSet *set, size_t index,
                                              uint64_t presented_revision);
    UmiStatus UmiDocumentReplacementSetCheck(const UmiDocumentReplacementSet *set);
    /* Require explicit approval and review of every changed document. Apply either
 * the entire captured set or none, using SourceBatch and per-document Undo.
 * This changes drafts only; saving and project-wide Undo remain separate. */
    UmiStatus UmiDocumentReplacementSetApply(UmiDocumentReplacementSet *set, uint64_t reviewed_revision,
                                             int approved);
#ifdef __cplusplus
}
#endif
#endif
