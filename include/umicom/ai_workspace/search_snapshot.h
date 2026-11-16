/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/search_snapshot.h
 *
 * PURPOSE:
 *   Own copied collection-search results and publish explicit ordered evidence selections without preparing or running a job.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKSPACE_SEARCH_SNAPSHOT_H
#define UMICOM_AI_WORKSPACE_SEARCH_SNAPSHOT_H
#include "umicom/ai_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiSearchSnapshot UmiAiSearchSnapshot;
typedef struct UmiAiSearchSummary {
    uint64_t workspaceRevision,corpusRevision;
    UmiAiWorkspaceCollection collection;
    char query[UMI_AI_WORKSPACE_PASSAGE_CAPACITY];
    char embeddingModelId[UMI_AI_ID_CAPACITY];
    size_t count,requestedLimit;
    bool vectorRequested;
} UmiAiSearchSummary;
typedef struct UmiAiSearchSelection {
    uint64_t corpusRevision;
    UmiAiWorkspaceCollection collection;
    char query[UMI_AI_WORKSPACE_PASSAGE_CAPACITY];
    size_t count;
    UmiAiWorkspaceEvidence evidence[UMI_AI_WORKSPACE_MAX_EVIDENCE];
} UmiAiSearchSelection;
/* Run the existing collection-scoped search once, then own its copied rows and
 * query metadata. resultLimit is 1..UMI_AI_MAX_RETRIEVAL_RESULTS. Zero matches
 * is an OK empty snapshot; unavailable requested vector evidence is an error.
 * Existing query-term scoring, finite reranker checks and stable ordering are
 * preserved. No index, embedding encoder or alternate search is introduced.
 *
 * Use the workspace's owner thread; inputs and trusted synchronous callbacks
 * remain alive/stable until return. The workspace prevents mutation during
 * reranking. Callback external effects cannot be rolled back. No provider,
 * filesystem, preparation, review or job execution is performed by this call.
 *
 * Initialize *out=NULL. All failures preserve it. The successful snapshot owns
 * query/source text, can outlive the workspace, and does not refresh after source
 * changes. It records the loaded corpus, not a newly reloaded database view.
 * Scores and query metadata are not authenticated identities or permissions. */
UmiStatus UmiAiWorkspaceSearchCapture(UmiAiWorkspace *workspace,
    const char *collectionId,const char *query,const char *embeddingModelId,
    const UmiAiEmbedding *queryEmbedding,const UmiAiReranker *reranker,
    size_t resultLimit,UmiAiSearchSnapshot **out);
void UmiAiSearchSnapshotDestroy(UmiAiSearchSnapshot *snapshot);
/* Read immutable copied values. Failure preserves caller output. Readers may
 * overlap while the owner keeps the snapshot alive; destruction waits for them.
 * Output storage must not overlap the opaque owner. */
UmiStatus UmiAiSearchSnapshotInspect(const UmiAiSearchSnapshot *snapshot,UmiAiSearchSummary *out);
UmiStatus UmiAiSearchSnapshotAt(const UmiAiSearchSnapshot *snapshot,size_t index,UmiAiWorkspaceEvidence *out);
/* Choose 1..UMI_AI_WORKSPACE_MAX_EVIDENCE unique zero-based row indices, keeping
 * the supplied order. Missing indices return NOT_FOUND; duplicates return
 * ALREADY_EXISTS; invalid/zero count returns INVALID_ARGUMENT. Every failure
 * preserves the complete output. indices may borrow from the previous output.
 *
 * The copied selection contains the original corpus revision and exact source
 * bytes for UmiAiWorkspacePrepareEvidence. Selection itself performs no current
 * source check and grants no authority. Preparation must recheck that revision
 * and all selected records; later review/execution remain separate operations. */
UmiStatus UmiAiSearchSnapshotSelect(const UmiAiSearchSnapshot *snapshot,
    const size_t *indices,size_t count,UmiAiSearchSelection *out);
#ifdef __cplusplus
}
#endif
#endif
