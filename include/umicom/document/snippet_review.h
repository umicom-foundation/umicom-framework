/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/snippet_review.h
 * PURPOSE: Review linked snippet values against one captured draft before applying a single undoable insertion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SNIPPET_REVIEW_H
#define UMICOM_DOCUMENT_SNIPPET_REVIEW_H
#include "umicom/document/source_request.h"
#include "umicom/editor/snippet_session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDocumentSnippetReview UmiDocumentSnippetReview;
    typedef struct UmiDocumentSnippetSummary
    {
        UmiDocumentSourceRequestSummary source;
        uint64_t revision;
        size_t placeholder_count, expanded_bytes;
        int has_template, prepared, applied;
    } UmiDocumentSnippetSummary;
    /* Own an immutable draft capture and an independent snippet session. All calls
 * belong on the coordinator's owning thread. Keep that coordinator, document
 * store and workbench alive until destruction; no GTK widget is borrowed.
 * Creation refuses read-only documents and clears the output on failure. */
    UmiStatus UmiDocumentSnippetReviewCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                             UmiDocumentSnippetReview **out_review);
    void UmiDocumentSnippetReviewDestroy(UmiDocumentSnippetReview *review);
    UmiStatus UmiDocumentSnippetReviewInspect(const UmiDocumentSnippetReview *review,
                                              UmiDocumentSnippetSummary *out_summary);
    /* A successful edit advances the review revision and expires any prepared
 * insertion. Failed edits preserve the earlier values and proposal. Template
 * syntax follows snippet_session: numbered values, literal defaults, choices
 * and $0. This is not a complete language-server snippet grammar. The template
 * and each replacement are copied; linked occurrences update together. */
    UmiStatus UmiDocumentSnippetReviewTemplate(UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                               const UmiEditorSnippetTemplate *snippet,
                                               const UmiCancellationToken *cancel);
    UmiStatus UmiDocumentSnippetReviewValue(UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                            uint32_t ordinal, const char *text, size_t bytes,
                                            const UmiCancellationToken *cancel);
    UmiStatus UmiDocumentSnippetReviewPlaceholder(const UmiDocumentSnippetReview *review, size_t index,
                                                  UmiEditorSnippetPlaceholder *out_placeholder);
    /* Borrow full original, expanded or proposed UTF-8 text. Original lasts until
 * destruction; expanded/proposed views expire on a successful mutation or
 * destruction. Outputs clear on failure. Copy before callbacks that can edit
 * or close the review. Proposed is available only after Prepare and before Apply. */
    UmiStatus UmiDocumentSnippetReviewOriginal(const UmiDocumentSnippetReview *review, const char **out_text,
                                               size_t *out_bytes);
    UmiStatus UmiDocumentSnippetReviewExpanded(const UmiDocumentSnippetReview *review, const char **out_text,
                                               size_t *out_bytes);
    UmiStatus UmiDocumentSnippetReviewProposed(const UmiDocumentSnippetReview *review, const char **out_text,
                                               size_t *out_bytes);
    /* Prepare replaces the captured selection, or inserts at the captured caret.
 * It rechecks the source, builds the complete proposed draft and advances the
 * review revision. The first $0 sets the final caret; otherwise the caret follows
 * the expanded text. It changes no live source, history, active tab or saved file.
 * Every successful preparation requires fresh explicit approval. */
    UmiStatus UmiDocumentSnippetReviewPrepare(UmiDocumentCoordinator *coordinator,
                                              UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                              const UmiCancellationToken *cancel);
    UmiStatus UmiDocumentSnippetReviewCheck(UmiDocumentCoordinator *coordinator,
                                            const UmiDocumentSnippetReview *review);
    /* Apply rechecks identity, content, caret, permissions and save/conflict state.
 * Only the current prepared revision can be approved. One Undo restores the
 * complete captured draft; prior pending typing keeps its separate Undo step.
 * Success consumes the review. No save or provider request is performed. */
    UmiStatus UmiDocumentSnippetReviewApply(UmiDocumentCoordinator *coordinator,
                                            UmiDocumentSnippetReview *review, uint64_t reviewed_revision,
                                            int approved);
#ifdef __cplusplus
}
#endif
#endif
