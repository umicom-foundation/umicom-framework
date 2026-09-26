/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/workspace.h
 *
 * PURPOSE:
 *   Compose source collections and reviewed AI jobs over the canonical AI Runtime and Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_WORKSPACE_H
#define UMICOM_AI_WORKSPACE_WORKSPACE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/runtime.h"
#include "umicom/ai/hybrid_retrieval.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_WORKSPACE_MAX_COLLECTIONS 16U
#define UMI_AI_WORKSPACE_MAX_SOURCES 64U
#define UMI_AI_WORKSPACE_MAX_JOBS 32U
#define UMI_AI_WORKSPACE_MAX_EVIDENCE 4U
#define UMI_AI_WORKSPACE_ID_CAPACITY 64U
#define UMI_AI_WORKSPACE_TITLE_CAPACITY 192U
#define UMI_AI_WORKSPACE_PASSAGE_CAPACITY 1536U

/* These are copied views, not handles into the database or model runtime. */
typedef struct UmiAiWorkspaceCollection {
    char id[UMI_AI_WORKSPACE_ID_CAPACITY];
    char title[UMI_AI_WORKSPACE_TITLE_CAPACITY];
} UmiAiWorkspaceCollection;

typedef struct UmiAiWorkspaceSource {
    char id[UMI_AI_WORKSPACE_ID_CAPACITY];
    char collectionId[UMI_AI_WORKSPACE_ID_CAPACITY];
    char title[UMI_AI_WORKSPACE_TITLE_CAPACITY];
    char text[UMI_AI_WORKSPACE_PASSAGE_CAPACITY];
    uint32_t firstLine;
    uint32_t lastLine;
    uint64_t revision;
} UmiAiWorkspaceSource;

typedef struct UmiAiWorkspaceEvidence {
    UmiAiWorkspaceSource source;
    double score; /* Ranking aid, never a probability that an answer is correct. */
} UmiAiWorkspaceEvidence;

typedef enum UmiAiWorkspaceJobKind {
    UMI_AI_WORKSPACE_DRAFT = 1,
    UMI_AI_WORKSPACE_GROUNDED_DRAFT = 2,
    UMI_AI_WORKSPACE_TOOL = 3
} UmiAiWorkspaceJobKind;

typedef enum UmiAiWorkspaceJobState {
    UMI_AI_WORKSPACE_REVIEW = 1,
    UMI_AI_WORKSPACE_APPROVED = 2,
    UMI_AI_WORKSPACE_RUNNING = 3,
    UMI_AI_WORKSPACE_SUCCEEDED = 4,
    UMI_AI_WORKSPACE_FAILED = 5,
    UMI_AI_WORKSPACE_DENIED = 6,
    UMI_AI_WORKSPACE_CANCELLED = 7,
    UMI_AI_WORKSPACE_INTERRUPTED = 8
} UmiAiWorkspaceJobState;

typedef struct UmiAiWorkspaceJob {
    char id[UMI_AI_WORKSPACE_ID_CAPACITY];
    char providerId[UMI_AI_ID_CAPACITY]; /* A tool ID for TOOL jobs. */
    char modelId[UMI_AI_ID_CAPACITY];
    char collectionId[UMI_AI_WORKSPACE_ID_CAPACITY];
    char prompt[UMI_AI_WORKSPACE_PASSAGE_CAPACITY]; /* JSON arguments for TOOL. */
    char requestedBy[UMI_AI_WORKSPACE_ID_CAPACITY];
    char reviewedBy[UMI_AI_WORKSPACE_ID_CAPACITY];
    char permission[UMI_AI_ID_CAPACITY];
    UmiAiWorkspaceJobKind kind;
    UmiAiWorkspaceJobState state;
    UmiAiProviderKind providerKind;
    uint32_t maxOutputTokens;
    uint64_t corpusRevision;
    size_t evidenceCount;
    UmiAiWorkspaceEvidence evidence[UMI_AI_WORKSPACE_MAX_EVIDENCE];
    UmiAiResponse response;
    UmiStatus status;
} UmiAiWorkspaceJob;

typedef struct UmiAiWorkspaceSnapshot {
    uint64_t revision;
    uint64_t corpusRevision;
    size_t collectionCount;
    size_t sourceCount;
    size_t jobCount;
    bool busy;
    bool recoveryRequired;
} UmiAiWorkspaceSnapshot;

typedef struct UmiAiWorkspace UmiAiWorkspace;
typedef struct UmiAiWorkspaceCancellation UmiAiWorkspaceCancellation;
/* Only Request and IsRequested may run concurrently with an operation.
 * Reset and Destroy require all work using this token to have finished. */
UmiStatus UmiAiWorkspaceCancellationCreate(UmiAiWorkspaceCancellation **outCancellation);
void UmiAiWorkspaceCancellationRequest(UmiAiWorkspaceCancellation *cancellation);
void UmiAiWorkspaceCancellationReset(UmiAiWorkspaceCancellation *cancellation);
bool UmiAiWorkspaceCancellationIsRequested(const UmiAiWorkspaceCancellation *cancellation);
void UmiAiWorkspaceCancellationDestroy(UmiAiWorkspaceCancellation *cancellation);

/* Borrow the Data Server and runtime. They must outlive this workspace. Use a
 * dedicated Data Server connection and one owner thread. No other caller may
 * mutate the borrowed registries during a workspace operation. Construction
 * reads only: it does not create records, contact a provider or run a job.
 * User labels below are local review records, not authenticated identities. */
UmiStatus UmiAiWorkspaceCreate(UmiDataServer *data, UmiAiRuntime *runtime,
    const char *bookId, UmiAiWorkspace **outWorkspace);
void UmiAiWorkspaceDestroy(UmiAiWorkspace *workspace);
UmiStatus UmiAiWorkspaceReload(UmiAiWorkspace *workspace);
UmiStatus UmiAiWorkspaceSnapshotRead(const UmiAiWorkspace *workspace,
    UmiAiWorkspaceSnapshot *outSnapshot);
UmiStatus UmiAiWorkspaceCollectionAt(const UmiAiWorkspace *workspace, size_t index,
    UmiAiWorkspaceCollection *outCollection);
UmiStatus UmiAiWorkspaceSourceAt(const UmiAiWorkspace *workspace, size_t index,
    UmiAiWorkspaceSource *outSource);
UmiStatus UmiAiWorkspaceJobAt(const UmiAiWorkspace *workspace, size_t index,
    UmiAiWorkspaceJob *outJob);
UmiStatus UmiAiWorkspaceJobFind(const UmiAiWorkspace *workspace, const char *id,
    UmiAiWorkspaceJob *outJob);

/* Mutations commit atomically and reject stale writers. Exact repeated inputs
 * do not create another revision. Text is UTF-8; capacities count bytes, not
 * displayed characters. A passage is explicitly selected text, not a PDF or
 * an instruction to scan the filesystem. Lines are counted from firstLine. */
UmiStatus UmiAiWorkspacePutCollection(UmiAiWorkspace *workspace, const char *id,
    const char *title);
UmiStatus UmiAiWorkspacePutSource(UmiAiWorkspace *workspace, const char *id,
    const char *collectionId, const char *title, const char *text,
    uint32_t firstLine);
UmiStatus UmiAiWorkspaceRemoveSource(UmiAiWorkspace *workspace, const char *id);

/* Optional embeddings are copied into a non-durable, revision-bound cache.
 * They must use the same model ID and dimension as the query. Reload and any
 * corpus change invalidate this cache. No embeddings are invented from hashes
 * or described as model inference. The existing canonical 64-value reference
 * embedding limit applies. Rerankers are trusted, synchronous callbacks. */
UmiStatus UmiAiWorkspaceSetEmbedding(UmiAiWorkspace *workspace,
    const char *sourceId, uint64_t sourceRevision, const char *modelId,
    const UmiAiEmbedding *embedding);
UmiStatus UmiAiWorkspaceSearch(UmiAiWorkspace *workspace, const char *collectionId,
    const char *query, const char *embeddingModelId,
    const UmiAiEmbedding *queryEmbedding, const UmiAiReranker *reranker,
    UmiAiWorkspaceEvidence *results, size_t capacity, size_t *outCount);

/* Preparation freezes the selected passages and complete request. Reusing a
 * job ID with different input fails; the job is never silently overwritten.
 * Default preparation uses lexical retrieval; a custom search can be inspected
 * separately. Every job, including local generation, requires explicit review.
 * TOOL preparation additionally requires an explicit tool allowlist entry. */
UmiStatus UmiAiWorkspacePrepare(UmiAiWorkspace *workspace, const char *jobId,
    UmiAiWorkspaceJobKind kind, const char *providerId, const char *modelId,
    const char *collectionId, const char *prompt, const char *requestedBy,
    uint32_t maxOutputTokens);
UmiStatus UmiAiWorkspaceAllowTool(UmiAiWorkspace *workspace, const char *toolId);
UmiStatus UmiAiWorkspaceReview(UmiAiWorkspace *workspace, const char *jobId,
    const char *reviewedBy, bool approve);
UmiStatus UmiAiWorkspaceCancel(UmiAiWorkspace *workspace, const char *jobId);
UmiStatus UmiAiWorkspaceBuildRequest(const UmiAiWorkspaceJob *job,
    UmiAiRequest *outRequest);

/* Synchronous execution on the owner's thread. RUNNING is durably committed
 * before a callback is invoked. No database transaction spans model/tool work.
 * cancellation may be set from another thread; all other workspace access must
 * wait until Run returns. Provider callbacks must be bounded. A failed final
 * commit sets recoveryRequired and must not trigger an automatic retry. */
UmiStatus UmiAiWorkspaceRun(UmiAiWorkspace *workspace, const char *jobId,
    const UmiAiWorkspaceCancellation *cancellation);
const char *UmiAiWorkspaceJobStateText(UmiAiWorkspaceJobState state);

#ifdef __cplusplus
}
#endif
#endif
