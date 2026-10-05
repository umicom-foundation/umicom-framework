/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/replacement_session.h
 * PURPOSE: Review literal replacements across captured open documents one draft at a time.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_REPLACEMENT_SESSION_H
#define UMICOM_DOCUMENT_REPLACEMENT_SESSION_H
#include "umicom/document/replacement.h"
#include "umicom/editor/search_engine.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDocumentReplacementPhase {
    UMI_DOCUMENT_REPLACEMENT_READY = 1,
    UMI_DOCUMENT_REPLACEMENT_REVIEW,
    UMI_DOCUMENT_REPLACEMENT_COMPLETE,
    UMI_DOCUMENT_REPLACEMENT_CANCELLED,
    UMI_DOCUMENT_REPLACEMENT_FAILED
} UmiDocumentReplacementPhase;

typedef enum UmiDocumentReplacementDecision {
    UMI_DOCUMENT_REPLACEMENT_APPLY = 1,
    UMI_DOCUMENT_REPLACEMENT_SKIP,
    UMI_DOCUMENT_REPLACEMENT_STOP
} UmiDocumentReplacementDecision;

/* Counts partition total: applied + skipped + unchanged + read_only + closed
 * + remaining. applied counts changed drafts; replacements counts matches in
 * those accepted drafts. current contains no source text. Earlier accepted
 * edits remain when a later document fails or the user stops the sequence. */
typedef struct UmiDocumentReplacementProgress {
    UmiDocumentReplacementPhase phase;
    UmiStatus last_status;
    size_t total;
    size_t applied;
    size_t skipped;
    size_t unchanged;
    size_t read_only;
    size_t closed;
    size_t remaining;
    size_t replacements;
    UmiDocumentReplacementSummary current;
} UmiDocumentReplacementProgress;

typedef struct UmiDocumentReplacementSession UmiDocumentReplacementSession;

/* Capture current document IDs, original paths and opening order. Documents
 * opened later are excluded; pinned and untitled drafts are included. Copies
 * the UTF-8 literal needle and replacement, using the existing smart-case
 * semantics. Empty needles are invalid; empty replacement removes matches.
 * Inputs use the existing document byte limit. Clears *outSession on failure.
 * Borrows the coordinator through the last operation. Use its owning thread
 * and keep its store/workbench alive. No file read, write, tab activation or
 * draft edit occurs during creation. Only one full text plan is retained. */
UmiStatus UmiDocumentReplacementSessionCreate(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, UmiDocumentReplacementSession **outSession);
/** Capture explicit case and whole-word policy for every reviewed draft.
 * NULL retains smart-case defaults. Later changes to caller options cannot
 * alter this capture; complete replacement ignores match-count/overlap limits. */
UmiStatus UmiDocumentReplacementSessionCreateWithOptions(UmiDocumentCoordinator *coordinator, const char *needle,
    const char *replacement, const UmiEditorSearchOptions *options, UmiDocumentReplacementSession **outSession);
void UmiDocumentReplacementSessionDestroy(UmiDocumentReplacementSession *session);
UmiStatus UmiDocumentReplacementSessionProgress(const UmiDocumentReplacementSession *session,
    UmiDocumentReplacementProgress *outProgress);

/* Inspect at most one captured identity. Closed targets, read-only drafts and
 * unchanged results advance their explicit counters. A changed path/view or
 * preparation error stops the sequence. A matching writable draft enters REVIEW.
 * Changes before a document's turn appear in its new review; changes after its
 * question is captured invalidate Apply. Calling Step during REVIEW does not
 * replace the question. Inspect progress after OK, which need not mean complete. */
UmiStatus UmiDocumentReplacementSessionStep(UmiDocumentReplacementSession *session);

/* Borrow the exact before/after buffers only in REVIEW. Valid until Respond,
 * Cancel or Destroy. Clear supplied text outputs on errors. A GUI must copy the
 * text if its widgets will outlive this question. No text is truncated. */
UmiStatus UmiDocumentReplacementSessionTexts(const UmiDocumentReplacementSession *session,
    const char **outPrevious, size_t *outPreviousBytes,
    const char **outProposed, size_t *outProposedBytes);
UmiStatus UmiDocumentReplacementSessionCheck(const UmiDocumentReplacementSession *session);

/* APPLY uses the existing reviewed replacement and per-document Undo. SKIP
 * never changes the draft, even if it changed since capture. STOP cancels all
 * remaining work. No automatic Apply All, saving, cross-document rollback or
 * project-wide disk replacement is performed. Invalid decisions preserve the
 * question; a failed Apply stops later work and keeps earlier accepted edits. */
UmiStatus UmiDocumentReplacementSessionRespond(UmiDocumentReplacementSession *session,
    UmiDocumentReplacementDecision decision);
/* Stop remaining work; terminal sessions are left intact. Safe without the
 * coordinator because cancellation releases only owned captures and text. */
UmiStatus UmiDocumentReplacementSessionCancel(UmiDocumentReplacementSession *session);
#ifdef __cplusplus
}
#endif
#endif
