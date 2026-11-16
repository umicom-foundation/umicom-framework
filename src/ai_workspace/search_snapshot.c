/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/search_snapshot.c
 *
 * PURPOSE:
 *   Compose canonical workspace retrieval into an immutable search owner and atomic selected-source value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workspace/search_snapshot.h"
#include "umicom/security/secrets.h"
#include "workspace_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiAiSearchSnapshot {
    UmiAiSearchSummary summary;
    UmiAiWorkspaceEvidence rows[UMI_AI_MAX_RETRIEVAL_RESULTS];
};
void UmiAiSearchSnapshotDestroy(UmiAiSearchSnapshot *snapshot)
{
    if(snapshot!=NULL){umi_secret_clear(snapshot,sizeof *snapshot);free(snapshot);}
}
UmiStatus UmiAiWorkspaceSearchCapture(UmiAiWorkspace *workspace,
    const char *collectionId,const char *query,const char *embeddingModelId,
    const UmiAiEmbedding *queryEmbedding,const UmiAiReranker *reranker,
    size_t resultLimit,UmiAiSearchSnapshot **out)
{
    if(out==NULL || *out!=NULL || resultLimit==0U || resultLimit>UMI_AI_MAX_RETRIEVAL_RESULTS)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=AwReady(workspace);
    if(status!=UMI_STATUS_OK)return status;
    if(!AwIdValid(collectionId,UMI_AI_WORKSPACE_ID_CAPACITY) ||
        !AwTextValid(query,UMI_AI_WORKSPACE_PASSAGE_CAPACITY,false))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t collection=AwCollectionIndex(workspace->state,collectionId);
    if(collection==SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    UmiAiSearchSnapshot *candidate=calloc(1U,sizeof *candidate);
    if(candidate==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    candidate->summary.workspaceRevision=workspace->state->revision;
    candidate->summary.corpusRevision=workspace->state->corpusRevision;
    candidate->summary.collection=workspace->state->collections[collection];
    candidate->summary.requestedLimit=resultLimit;
    candidate->summary.vectorRequested=queryEmbedding!=NULL;
    status=AwTextCopy(candidate->summary.query,sizeof candidate->summary.query,query,false);
    if(status==UMI_STATUS_OK && queryEmbedding!=NULL)
        status=AwTextCopy(candidate->summary.embeddingModelId,
            sizeof candidate->summary.embeddingModelId,embeddingModelId,false);
    /* Canonical retrieval validates vector shape, reranker admission and all
     * selected source provenance. The candidate remains unpublished on error. */
    if(status==UMI_STATUS_OK)
        status=UmiAiWorkspaceSearch(workspace,collectionId,query,embeddingModelId,
            queryEmbedding,reranker,candidate->rows,resultLimit,&candidate->summary.count);
    if(status==UMI_STATUS_OK &&
        (candidate->summary.count>resultLimit ||
         candidate->summary.corpusRevision!=workspace->state->corpusRevision))
        status=UMI_STATUS_BUSY;
    if(status==UMI_STATUS_OK){*out=candidate;candidate=NULL;}
    UmiAiSearchSnapshotDestroy(candidate);return status;
}
UmiStatus UmiAiSearchSnapshotInspect(const UmiAiSearchSnapshot *snapshot,UmiAiSearchSummary *out)
{
    if(snapshot==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=snapshot->summary;return UMI_STATUS_OK;
}
UmiStatus UmiAiSearchSnapshotAt(const UmiAiSearchSnapshot *snapshot,size_t index,UmiAiWorkspaceEvidence *out)
{
    if(snapshot==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=snapshot->summary.count)return UMI_STATUS_NOT_FOUND;
    *out=snapshot->rows[index];return UMI_STATUS_OK;
}
UmiStatus UmiAiSearchSnapshotSelect(const UmiAiSearchSnapshot *snapshot,
    const size_t *indices,size_t count,UmiAiSearchSelection *out)
{
    if(snapshot==NULL || indices==NULL || out==NULL ||
        count==0U || count>UMI_AI_WORKSPACE_MAX_EVIDENCE)return UMI_STATUS_INVALID_ARGUMENT;
    /* Admit the entire selection before constructing or publishing it. The
     * bounded local index copy also allows indices borrowed from old output. */
    size_t chosen[UMI_AI_WORKSPACE_MAX_EVIDENCE];
    for(size_t i=0U;i<count;++i){
        if(indices[i]>=snapshot->summary.count)return UMI_STATUS_NOT_FOUND;
        for(size_t n=0U;n<i;++n)if(chosen[n]==indices[i])return UMI_STATUS_ALREADY_EXISTS;
        chosen[i]=indices[i];
    }
    UmiAiSearchSelection candidate={0};
    candidate.corpusRevision=snapshot->summary.corpusRevision;
    candidate.collection=snapshot->summary.collection;
    memcpy(candidate.query,snapshot->summary.query,sizeof candidate.query);
    candidate.count=count;
    for(size_t i=0U;i<count;++i)candidate.evidence[i]=snapshot->rows[chosen[i]];
    *out=candidate;
    umi_secret_clear(&candidate,sizeof candidate);
    return UMI_STATUS_OK;
}
