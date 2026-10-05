/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_review/workspace_edit.c
 * PURPOSE: Resolve and approve complete language proposals without partial document publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/source_review/workspace_edit.h"
#include "umicom/document/source_batch.h"
#include <stdlib.h>
#include <string.h>
struct UmiSourceWorkspaceEditReview
{
    UmiDocumentCoordinator *coordinator;
    UmiDocumentSourceWorkspace *sources;
    UmiLanguageWorkspaceEditCatalogue *catalogue;
    UmiDocumentSourceBatch *batch;
    UmiSourceWorkspaceEditReviewSummary summary;
    unsigned char reviewed[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    unsigned char *required, *confirmed;
    size_t annotation_count;
};
void UmiSourceWorkspaceEditReviewDestroy(UmiSourceWorkspaceEditReview *review)
{
    if (review == NULL)
        return;
    UmiDocumentSourceBatchDestroy(review->batch);
    UmiDocumentSourceWorkspaceDestroy(review->sources);
    UmiLanguageWorkspaceEditCatalogueDestroy(review->catalogue);
    free(review->required);
    free(review->confirmed);
    free(review);
}
/* Use the shared cancellation token contract so document review observes the same cancellation request as native source queries.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiSourceWorkspaceEditReviewCreate(UmiDocumentCoordinator *coordinator,
    UmiDocumentSourceWorkspace **sources,UmiLanguageWorkspaceEditCatalogue **catalogue,
    const int32_t *source_version,const UmiCancellationToken *cancel,UmiSourceWorkspaceEditReview **out_review)
{
    if(out_review==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_review=NULL;
    if(coordinator==NULL || sources==NULL || *sources==NULL || catalogue==NULL || *catalogue==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(cancel!=NULL && umi_cancellation_token_is_cancelled(cancel)) return UMI_STATUS_CANCELLED;
    size_t count=UmiLanguageWorkspaceEditCatalogueCount(*catalogue);
    if(count==0U) return UMI_STATUS_NOT_FOUND;
    if(count>UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status=UmiDocumentSourceWorkspaceCheck(coordinator,*sources);
    if(status!=UMI_STATUS_OK) return status;
    UmiDocumentId ids[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    size_t indices[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    for(size_t i=0U;i<count;++i) {
        UmiLanguageWorkspaceDocumentChange change;
        UmiDocumentSourceRequestSummary document;
        status=UmiLanguageWorkspaceEditCatalogueDocument(*catalogue,i,&change);
        if(status==UMI_STATUS_OK) status=UmiDocumentSourceWorkspaceFind(*sources,change.uri,&indices[i]);
        if(status==UMI_STATUS_OK) status=UmiDocumentSourceWorkspaceAt(*sources,indices[i],&document);
        if(status!=UMI_STATUS_OK) return status;
        ids[i]=document.document_id;
    }
    UmiSourceWorkspaceEditReview *review=calloc(1U,sizeof(*review));
    if(review==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    review->coordinator=coordinator;
    review->annotation_count=UmiLanguageWorkspaceEditCatalogueAnnotationCount(*catalogue);
    if(review->annotation_count!=0U) {
        review->required=calloc(review->annotation_count,1U);
        review->confirmed=calloc(review->annotation_count,1U);
        if(review->required==NULL || review->confirmed==NULL) status=UMI_STATUS_OUT_OF_MEMORY;
    }
    if(status==UMI_STATUS_OK) status=UmiDocumentSourceBatchCreate(coordinator,ids,count,&review->batch);
    for(size_t i=0U;status==UMI_STATUS_OK && i<count;++i) {
        if(cancel!=NULL && umi_cancellation_token_is_cancelled(cancel)) {status=UMI_STATUS_CANCELLED;break;}
        const char *source=NULL,*proposed=NULL;
        size_t source_bytes=0U,proposed_bytes=0U,proposed_caret=0U;
        UmiDocumentSourceRequestSummary document;
        UmiDocumentSourceBatchSummary batch;
        UmiLanguageWorkspaceDocumentChange change;
        UmiLanguageTextEditPreview *preview=NULL;
        status=UmiDocumentSourceWorkspaceAt(*sources,indices[i],&document);
        if(status==UMI_STATUS_OK) status=UmiDocumentSourceWorkspaceRead(*sources,indices[i],&source,&source_bytes);
        if(status==UMI_STATUS_OK) status=UmiLanguageWorkspaceEditCataloguePreview(*catalogue,i,source,
            source_bytes,document.cursor_offset,source_version,cancel,&preview);
        if(status==UMI_STATUS_OK) status=UmiLanguageTextEditPreviewRead(preview,&proposed,&proposed_bytes,&proposed_caret);
        if(status==UMI_STATUS_OK) status=UmiDocumentSourceBatchInspect(review->batch,&batch);
        if(status==UMI_STATUS_OK) status=UmiDocumentSourceBatchStage(review->batch,i,batch.revision,
            proposed,proposed_bytes,proposed_caret);
        UmiLanguageTextEditPreviewDestroy(preview);
        if(status==UMI_STATUS_OK) status=UmiLanguageWorkspaceEditCatalogueDocument(*catalogue,i,&change);
        /* An annotation is a separate permission gate, even when its referenced
         * text edit happens to leave the bytes unchanged. Unused annotations
         * remain available for inspection but acquire no artificial gate. */
        for(size_t j=0U;status==UMI_STATUS_OK && j<change.edit_count;++j) {
            UmiLanguageWorkspaceTextChange edit;
            status=UmiLanguageWorkspaceEditCatalogueEdit(*catalogue,i,j,&edit);
            if(status!=UMI_STATUS_OK || edit.annotation==SIZE_MAX) continue;
            UmiLanguageWorkspaceChangeAnnotation annotation;
            status=UmiLanguageWorkspaceEditCatalogueAnnotation(*catalogue,edit.annotation,&annotation);
            if(status==UMI_STATUS_OK && annotation.needs_confirmation && !review->required[edit.annotation]) {
                review->required[edit.annotation]=1U;++review->summary.required_annotations;
            }
        }
    }
    UmiDocumentSourceBatchSummary batch;
    if(status==UMI_STATUS_OK) status=UmiDocumentSourceBatchInspect(review->batch,&batch);
    if(status==UMI_STATUS_OK) status=UmiDocumentSourceWorkspaceCheck(coordinator,*sources);
    if(status==UMI_STATUS_OK && cancel!=NULL && umi_cancellation_token_is_cancelled(cancel)) status=UMI_STATUS_CANCELLED;
    if(status!=UMI_STATUS_OK) {UmiSourceWorkspaceEditReviewDestroy(review);return status;}
    review->summary.dependency_count=UmiDocumentSourceWorkspaceCount(*sources);
    review->summary.document_count=count;review->summary.changed_count=batch.changed_count;
    review->summary.revision=batch.revision;
    /* Transfer only after every fallible preparation has succeeded. The caller
     * retains both inputs on failure and never needs to guess who frees them. */
    review->sources=*sources;review->catalogue=*catalogue;
    *sources=NULL;*catalogue=NULL;*out_review=review;return UMI_STATUS_OK;
}
#endif
UmiStatus UmiSourceWorkspaceEditReviewCreate(UmiDocumentCoordinator *coordinator,
                                             UmiDocumentSourceWorkspace **sources,
                                             UmiLanguageWorkspaceEditCatalogue **catalogue,
                                             const int32_t *source_version,
                                             const UmiCancellationToken *cancel,
                                             UmiSourceWorkspaceEditReview **out_review)
{
    if (out_review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_review = NULL;
    if (coordinator == NULL || sources == NULL || *sources == NULL || catalogue == NULL || *catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (cancel != NULL && umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    size_t count = UmiLanguageWorkspaceEditCatalogueCount(*catalogue);
    if (count == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (count > UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiDocumentSourceWorkspaceCheck(coordinator, *sources);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDocumentId ids[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    size_t indices[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    for (size_t i = 0U; i < count; ++i)
    {
        UmiLanguageWorkspaceDocumentChange change;
        UmiDocumentSourceRequestSummary document;
        status = UmiLanguageWorkspaceEditCatalogueDocument(*catalogue, i, &change);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceWorkspaceFind(*sources, change.uri, &indices[i]);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceWorkspaceAt(*sources, indices[i], &document);
        if (status != UMI_STATUS_OK)
            return status;
        ids[i] = document.document_id;
    }
    UmiSourceWorkspaceEditReview *review = calloc(1U, sizeof(*review));
    if (review == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    review->coordinator = coordinator;
    review->annotation_count = UmiLanguageWorkspaceEditCatalogueAnnotationCount(*catalogue);
    if (review->annotation_count != 0U)
    {
        review->required = calloc(review->annotation_count, 1U);
        review->confirmed = calloc(review->annotation_count, 1U);
        if (review->required == NULL || review->confirmed == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchCreate(coordinator, ids, count, &review->batch);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        if (cancel != NULL && umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const char *source = NULL, *proposed = NULL;
        size_t source_bytes = 0U, proposed_bytes = 0U, proposed_caret = 0U;
        UmiDocumentSourceRequestSummary document;
        UmiDocumentSourceBatchSummary batch;
        UmiLanguageWorkspaceDocumentChange change;
        UmiLanguageTextEditPreview *preview = NULL;
        status = UmiDocumentSourceWorkspaceAt(*sources, indices[i], &document);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceWorkspaceRead(*sources, indices[i], &source, &source_bytes);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageWorkspaceEditCataloguePreview(*catalogue, i, source, source_bytes,
                                                              document.cursor_offset, source_version, cancel,
                                                              &preview);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageTextEditPreviewRead(preview, &proposed, &proposed_bytes, &proposed_caret);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceBatchInspect(review->batch, &batch);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceBatchStage(review->batch, i, batch.revision, proposed, proposed_bytes,
                                                 proposed_caret);
        UmiLanguageTextEditPreviewDestroy(preview);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageWorkspaceEditCatalogueDocument(*catalogue, i, &change);
        /* An annotation is a separate permission gate, even when its referenced
         * text edit happens to leave the bytes unchanged. Unused annotations
         * remain available for inspection but acquire no artificial gate. */
        for (size_t j = 0U; status == UMI_STATUS_OK && j < change.edit_count; ++j)
        {
            UmiLanguageWorkspaceTextChange edit;
            status = UmiLanguageWorkspaceEditCatalogueEdit(*catalogue, i, j, &edit);
            if (status != UMI_STATUS_OK || edit.annotation == SIZE_MAX)
                continue;
            UmiLanguageWorkspaceChangeAnnotation annotation;
            status = UmiLanguageWorkspaceEditCatalogueAnnotation(*catalogue, edit.annotation, &annotation);
            if (status == UMI_STATUS_OK && annotation.needs_confirmation &&
                !review->required[edit.annotation])
            {
                review->required[edit.annotation] = 1U;
                ++review->summary.required_annotations;
            }
        }
    }
    UmiDocumentSourceBatchSummary batch;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchInspect(review->batch, &batch);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceWorkspaceCheck(coordinator, *sources);
    if (status == UMI_STATUS_OK && cancel != NULL && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        UmiSourceWorkspaceEditReviewDestroy(review);
        return status;
    }
    review->summary.dependency_count = UmiDocumentSourceWorkspaceCount(*sources);
    review->summary.document_count = count;
    review->summary.changed_count = batch.changed_count;
    review->summary.revision = batch.revision;
    /* Transfer only after every fallible preparation has succeeded. The caller
     * retains both inputs on failure and never needs to guess who frees them. */
    review->sources = *sources;
    review->catalogue = *catalogue;
    *sources = NULL;
    *catalogue = NULL;
    *out_review = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiSourceWorkspaceEditReviewInspect(const UmiSourceWorkspaceEditReview *review,
                                              UmiSourceWorkspaceEditReviewSummary *out_summary)
{
    if (review == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = review->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiSourceWorkspaceEditReviewAt(const UmiSourceWorkspaceEditReview *review, size_t index,
                                         UmiDocumentSourceRequestSummary *out_document, int *out_reviewed)
{
    if (review == NULL || out_document == NULL || out_reviewed == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDocumentSourceBatchAt(review->batch, index, out_document);
    if (status == UMI_STATUS_OK)
        *out_reviewed = review->reviewed[index] != 0U;
    return status;
}
UmiStatus UmiSourceWorkspaceEditReviewTexts(const UmiSourceWorkspaceEditReview *review, size_t index,
                                            const char **out_source, size_t *out_source_bytes,
                                            const char **out_proposed, size_t *out_proposed_bytes)
{
    if (out_source != NULL)
        *out_source = NULL;
    if (out_source_bytes != NULL)
        *out_source_bytes = 0U;
    if (out_proposed != NULL)
        *out_proposed = NULL;
    if (out_proposed_bytes != NULL)
        *out_proposed_bytes = 0U;
    if (review == NULL || out_source == NULL || out_source_bytes == NULL || out_proposed == NULL ||
        out_proposed_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *source = NULL, *proposed = NULL;
    size_t source_bytes = 0U, proposed_bytes = 0U;
    UmiStatus status = UmiDocumentSourceBatchRead(review->batch, index, &source, &source_bytes);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchProposed(review->batch, index, &proposed, &proposed_bytes);
    if (status == UMI_STATUS_OK)
    {
        *out_source = source;
        *out_source_bytes = source_bytes;
        *out_proposed = proposed;
        *out_proposed_bytes = proposed_bytes;
    }
    return status;
}
UmiStatus UmiSourceWorkspaceEditReviewCatalogue(const UmiSourceWorkspaceEditReview *review,
                                                const UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    if (review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = review->catalogue;
    return UMI_STATUS_OK;
}
UmiStatus UmiSourceWorkspaceEditReviewCheck(const UmiSourceWorkspaceEditReview *review)
{
    if (review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (review->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiDocumentSourceWorkspaceCheck(review->coordinator, review->sources);
    return status == UMI_STATUS_OK ? UmiDocumentSourceBatchCheck(review->coordinator, review->batch) : status;
}
UmiStatus UmiSourceWorkspaceEditReviewAcceptDocument(UmiSourceWorkspaceEditReview *review, size_t index,
                                                     uint64_t presented_revision)
{
    if (review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->summary.document_count)
        return UMI_STATUS_NOT_FOUND;
    if (presented_revision != review->summary.revision)
        return UMI_STATUS_BUSY;
    UmiStatus status = UmiSourceWorkspaceEditReviewCheck(review);
    UmiDocumentSourceRequestSummary document;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchAt(review->batch, index, &document);
    if (status == UMI_STATUS_OK && !review->reviewed[index])
    {
        review->reviewed[index] = 1U;
        if (document.text_changes)
            ++review->summary.reviewed_count;
    }
    return status;
}
UmiStatus UmiSourceWorkspaceEditReviewAnnotationState(const UmiSourceWorkspaceEditReview *review,
                                                      size_t annotation_index, int *out_required,
                                                      int *out_confirmed)
{
    if (review == NULL || out_required == NULL || out_confirmed == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (annotation_index >= review->annotation_count)
        return UMI_STATUS_NOT_FOUND;
    *out_required = review->required[annotation_index] != 0U;
    *out_confirmed = review->confirmed[annotation_index] != 0U;
    return UMI_STATUS_OK;
}
UmiStatus UmiSourceWorkspaceEditReviewConfirmAnnotation(UmiSourceWorkspaceEditReview *review,
                                                        size_t annotation_index, uint64_t presented_revision,
                                                        int confirmed)
{
    if (review == NULL || (confirmed != 0 && confirmed != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (annotation_index >= review->annotation_count)
        return UMI_STATUS_NOT_FOUND;
    if (presented_revision != review->summary.revision)
        return UMI_STATUS_BUSY;
    if (!review->required[annotation_index])
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiSourceWorkspaceEditReviewCheck(review);
    if (status == UMI_STATUS_OK && (review->confirmed[annotation_index] != 0U) != confirmed)
    {
        review->confirmed[annotation_index] = (unsigned char)confirmed;
        if (confirmed)
            ++review->summary.confirmed_annotations;
        else
            --review->summary.confirmed_annotations;
    }
    return status;
}
UmiStatus UmiSourceWorkspaceEditReviewApply(UmiSourceWorkspaceEditReview *review, uint64_t reviewed_revision,
                                            int approved)
{
    if (review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (review->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != review->summary.revision)
        return UMI_STATUS_BUSY;
    if (review->summary.reviewed_count != review->summary.changed_count ||
        review->summary.confirmed_annotations != review->summary.required_annotations)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status = UmiSourceWorkspaceEditReviewCheck(review);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchApply(review->coordinator, review->batch, reviewed_revision, 1);
    if (status == UMI_STATUS_OK)
        review->summary.applied = 1;
    return status;
}

UmiStatus UmiSourceWorkspaceEditReviewCheckOwner(const UmiSourceWorkspaceEditReview *review,
                                                 UmiDocumentCoordinator *coordinator)
{
    if (review == NULL || coordinator == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (review->coordinator != coordinator)
        return UMI_STATUS_INVALID_STATE;
    return UmiSourceWorkspaceEditReviewCheck(review);
}
