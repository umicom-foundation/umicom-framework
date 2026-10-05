/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_review/workspace_edit.h
 * PURPOSE: Review a complete language workspace edit against immutable open-draft dependencies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SOURCE_REVIEW_WORKSPACE_EDIT_H
#define UMICOM_SOURCE_REVIEW_WORKSPACE_EDIT_H
#include "umicom/document/source_workspace.h"
#include "umicom/language_runtime/workspace_edit_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiSourceWorkspaceEditReview UmiSourceWorkspaceEditReview;
    typedef struct UmiSourceWorkspaceEditReviewSummary
    {
        size_t dependency_count, document_count, changed_count, reviewed_count;
        size_t required_annotations, confirmed_annotations;
        uint64_t revision;
        int applied;
    } UmiSourceWorkspaceEditReviewSummary;
    /* Compose document ownership with protocol decoding above both services.
 * Capture sources BEFORE requesting the provider result. This operation takes
 * ownership of both *sources and *catalogue only on success, clearing those
 * pointers. Failure leaves them with the caller and clears out_review.
 *
 * Every exact target URI must occur in sources; missing targets return NOT_FOUND
 * and refuse the complete proposal. Target drafts must be writable; unchanged
 * read-only dependencies are accepted. Resource operations have already been
 * refused by the catalogue. Empty catalogues return NOT_FOUND. No file opens,
 * edits, saves, provider calls or native event dispatch occur here.
 *
 * Supply the version actually sent with captured drafts, or NULL when unknown.
 * This shared version applies to every member and is never inferred from URI
 * or editor revisions. Fresh temporary queries send version 1. Versioned edits
 * require an exact match. Mixed protocol versions need another capture policy.
 * All ranges, overlaps and complete results are checked before review exists.
 *
 * Use the coordinator's owner thread and keep it, its store and workbench alive.
 * Preparation is bounded by SourceWorkspace, SourceBatch and CataloguePreview
 * limits. Cancellation cooperates between members and inside preview creation.
 * The result owns its captures, proposals and annotation data until Destroy. */
    UmiStatus UmiSourceWorkspaceEditReviewCreate(UmiDocumentCoordinator *coordinator,
                                                 UmiDocumentSourceWorkspace **sources,
                                                 UmiLanguageWorkspaceEditCatalogue **catalogue,
                                                 const int32_t *source_version,
                                                 const UmiCancellationToken *cancel,
                                                 UmiSourceWorkspaceEditReview **out_review);
    void UmiSourceWorkspaceEditReviewDestroy(UmiSourceWorkspaceEditReview *review);
    UmiStatus UmiSourceWorkspaceEditReviewInspect(const UmiSourceWorkspaceEditReview *review,
                                                  UmiSourceWorkspaceEditReviewSummary *out_summary);
    /* Text access follows the document transaction's consumed-state contract.
 * The earlier wording is retained for review because it incorrectly promised
 * post-application proposal access through SourceBatch.
 * Former wording: Document indices follow catalogue order. Metadata is copied.
 * Full immutable source and proposed text remain borrowed until Destroy,
 * including after Apply. Output text pointers/lengths clear on failure. */
    /* Document indices follow catalogue order. Metadata is copied. Texts borrows
 * immutable source and proposed text until Apply or Destroy. After Apply it
 * returns INVALID_STATE. Output text pointers/lengths clear on failure. */
    UmiStatus UmiSourceWorkspaceEditReviewAt(const UmiSourceWorkspaceEditReview *review, size_t index,
                                             UmiDocumentSourceRequestSummary *out_document,
                                             int *out_reviewed);
    UmiStatus UmiSourceWorkspaceEditReviewTexts(const UmiSourceWorkspaceEditReview *review, size_t index,
                                                const char **out_source, size_t *out_source_bytes,
                                                const char **out_proposed, size_t *out_proposed_bytes);
    /* The borrowed catalogue exposes complete edit ranges and annotation labels.
 * Do not destroy it or retain it beyond the review's lifetime. */
    UmiStatus UmiSourceWorkspaceEditReviewCatalogue(const UmiSourceWorkspaceEditReview *review,
                                                    const UmiLanguageWorkspaceEditCatalogue **out_catalogue);
    UmiStatus UmiSourceWorkspaceEditReviewCheck(const UmiSourceWorkspaceEditReview *review);
    /* Native hosts must match their bound coordinator before presenting a review.
 * This rejects a model from another workbench even when both remain alive. */
    UmiStatus UmiSourceWorkspaceEditReviewCheckOwner(const UmiSourceWorkspaceEditReview *review,
                                                     UmiDocumentCoordinator *coordinator);

    /* Call only after presenting this document's complete comparison. Every changed
 * document must be reviewed. Repeated acceptance does not count twice. */
    UmiStatus UmiSourceWorkspaceEditReviewAcceptDocument(UmiSourceWorkspaceEditReview *review, size_t index,
                                                         uint64_t presented_revision);
    /* Show annotation label/description and associated edits before confirming.
 * Only referenced annotations with needsConfirmation require a decision; an
 * unused annotation stays visible without blocking application. Confirmation
 * can be withdrawn. It does not approve the final application. */
    UmiStatus UmiSourceWorkspaceEditReviewAnnotationState(const UmiSourceWorkspaceEditReview *review,
                                                          size_t annotation_index, int *out_required,
                                                          int *out_confirmed);
    UmiStatus UmiSourceWorkspaceEditReviewConfirmAnnotation(UmiSourceWorkspaceEditReview *review,
                                                            size_t annotation_index,
                                                            uint64_t presented_revision, int confirmed);
    /* Check ALL captured sources, then apply ALL proposals through SourceBatch with
 * its preallocated view/store/Undo publication. Any failure applies no edits.
 * All changed documents, required annotations and final approval are mandatory.
 * Success consumes the review. Undo remains per-document and Save is separate. */
    UmiStatus UmiSourceWorkspaceEditReviewApply(UmiSourceWorkspaceEditReview *review,
                                                uint64_t reviewed_revision, int approved);
#ifdef __cplusplus
}
#endif
#endif
