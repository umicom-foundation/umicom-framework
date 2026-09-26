/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/retrieval.c
 *
 * PURPOSE:
 *   Scope canonical lexical/vector retrieval to a selected collection and preserve source provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workspace_internal.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct AwRerankGuard { const UmiAiReranker *delegate; bool invalid; } AwRerankGuard;
static double AwRerank(const char *query, const UmiAiChunk *chunk, double baseScore, void *context)
{
    AwRerankGuard *guard = context;
    double score = guard->delegate->score(query, chunk, baseScore, guard->delegate->user_data);
    if (!isfinite(score) || score < 0.0) { guard->invalid = true; return 0.0; }
    return score;
}
UmiStatus UmiAiWorkspaceSetEmbedding(UmiAiWorkspace *workspace, const char *sourceId,
    uint64_t sourceRevision, const char *modelId, const UmiAiEmbedding *embedding)
{
    UmiStatus status = AwReady(workspace); size_t index;
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(sourceId, UMI_AI_WORKSPACE_ID_CAPACITY) ||
        !AwTextValid(modelId, UMI_AI_ID_CAPACITY, false) || !AwEmbeddingValid(embedding)) return UMI_STATUS_INVALID_ARGUMENT;
    index = AwSourceIndex(workspace->state, sourceId); if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (workspace->state->sources[index].revision != sourceRevision) return UMI_STATUS_BUSY;
    AwEmbedding *cached = &workspace->embeddings[index]; memset(cached, 0, sizeof(*cached));
    cached->sourceRevision = sourceRevision; cached->value = *embedding;
    (void)AwTextCopy(cached->modelId, sizeof(cached->modelId), modelId, false);
    return UMI_STATUS_OK;
}
/* The legacy lexical primitive scores one literal term. This workspace adds
 * an opt-in query-term composition without changing that primitive's public
 * meaning. Short English function words are ignored; other scripts remain
 * literal UTF-8 terms. This is not linguistic stemming or a learned encoder. */
#define AW_MAX_QUERY_TERMS 32U
#define AW_TERM_CAPACITY 96U
static bool AwTermByte(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch >= 0x80U;
}
static bool AwStopWord(const char *word)
{
    static const char *const words[] = {"a", "an", "and", "are", "at", "be", "by", "do", "does",
        "for", "from", "how", "i", "in", "is", "it", "of", "on", "or", "that", "the", "this",
        "to", "was", "we", "what", "when", "where", "which", "who", "why", "with", "you"};
    for (size_t i = 0U; i < sizeof(words) / sizeof(words[0]); ++i)
        if (strcmp(word, words[i]) == 0) return true;
    return false;
}
static UmiStatus AwTerms(const char *query, char terms[AW_MAX_QUERY_TERMS][AW_TERM_CAPACITY], size_t *count)
{
    size_t offset = 0U; *count = 0U;
    while (query[offset] != '\0') {
        char term[AW_TERM_CAPACITY]; size_t length = 0U;
        while (query[offset] != '\0' && !AwTermByte((unsigned char)query[offset])) ++offset;
        while (AwTermByte((unsigned char)query[offset])) {
            if (length + 1U >= sizeof(term)) return UMI_STATUS_CAPACITY_EXCEEDED;
            unsigned char ch = (unsigned char)query[offset++];
            term[length++] = (char)(ch >= 'A' && ch <= 'Z' ? ch + ('a' - 'A') : ch);
        }
        term[length] = '\0';
        if (length == 0U || AwStopWord(term)) continue;
        bool duplicate = false;
        for (size_t i = 0U; i < *count; ++i) if (strcmp(term, terms[i]) == 0) duplicate = true;
        if (duplicate) continue;
        if (*count == AW_MAX_QUERY_TERMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(terms[(*count)++], term, length + 1U);
    }
    return *count != 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiAiWorkspaceSearch(UmiAiWorkspace *workspace, const char *collectionId,
    const char *query, const char *embeddingModelId, const UmiAiEmbedding *queryEmbedding,
    const UmiAiReranker *reranker, UmiAiWorkspaceEvidence *results, size_t capacity, size_t *outCount)
{
    UmiStatus status = AwReady(workspace);
    UmiAiRetrievalResult *ranked = NULL; size_t count = 0U, vectorCount = 0U, outputCount = 0U;
    char terms[AW_MAX_QUERY_TERMS][AW_TERM_CAPACITY]; size_t termCount = 0U;
    AwRerankGuard guard = {reranker, false}; UmiAiReranker guarded = {AwRerank, &guard};
    if (outCount != NULL) *outCount = 0U;
    if (status != UMI_STATUS_OK) return status;
    if (outCount == NULL || results == NULL || capacity == 0U || capacity > UMI_AI_MAX_RETRIEVAL_RESULTS ||
        !AwIdValid(collectionId, UMI_AI_WORKSPACE_ID_CAPACITY) ||
        !AwTextValid(query, UMI_AI_WORKSPACE_PASSAGE_CAPACITY, false) ||
        (reranker != NULL && reranker->score == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (AwCollectionIndex(workspace->state, collectionId) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (queryEmbedding != NULL && (!AwEmbeddingValid(queryEmbedding) ||
        !AwTextValid(embeddingModelId, UMI_AI_ID_CAPACITY, false))) return UMI_STATUS_INVALID_ARGUMENT;
    status = AwTerms(query, terms, &termCount);
    if (status != UMI_STATUS_OK) return status;
    ranked = calloc(UMI_AI_WORKSPACE_MAX_SOURCES, sizeof(*ranked));
    if (ranked == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Collection membership is checked before any scorer receives a passage.
     * The same canonical lexical scorer, cosine function and reranker serve
     * all three products; application code does not own a competing index. */
    for (size_t i = 0U; i < workspace->state->sourceCount; ++i) {
        const UmiAiWorkspaceSource *source = &workspace->state->sources[i];
        if (strcmp(source->collectionId, collectionId) != 0) continue;
        size_t hits = 0U;
        for (size_t t = 0U; t < termCount; ++t)
            if (umi_ai_retrieval_lexical_score(terms[t], source->text) > 0.0) ++hits;
        double lexicalScore = (double)hits / (double)termCount;
        double vectorScore = 0.0;
        const AwEmbedding *cached = &workspace->embeddings[i];
        if (queryEmbedding != NULL && cached->sourceRevision == source->revision &&
            cached->value.dimension == queryEmbedding->dimension && strcmp(cached->modelId, embeddingModelId) == 0) {
            vectorScore = umi_ai_embedding_cosine(queryEmbedding, &cached->value);
            if (vectorScore < 0.0) vectorScore = 0.0;
            ++vectorCount;
        }
        double score = queryEmbedding != NULL ? 0.5 * lexicalScore + 0.5 * vectorScore : lexicalScore;
        if (score <= 0.0) continue;
        UmiAiRetrievalResult *candidate = &ranked[count++];
        candidate->score = score;
        (void)AwTextCopy(candidate->chunk.chunk_id, sizeof(candidate->chunk.chunk_id), source->id, false);
        (void)AwTextCopy(candidate->chunk.document_id, sizeof(candidate->chunk.document_id), source->id, false);
        (void)AwTextCopy(candidate->chunk.text, sizeof(candidate->chunk.text), source->text, false);
        candidate->chunk.index = i; candidate->chunk.end_offset = strlen(source->text);
    }
    if (queryEmbedding != NULL && vectorCount == 0U) { free(ranked); return UMI_STATUS_UNAVAILABLE; }
    /* Canonical reranking is stable. Sorting IDs first fixes tie order even
     * when a source was removed and reinserted in a different storage slot. */
    for (size_t i = 1U; i < count; ++i) {
        UmiAiRetrievalResult entry = ranked[i]; size_t j = i;
        while (j > 0U && strcmp(ranked[j - 1U].chunk.chunk_id, entry.chunk.chunk_id) > 0) {
            ranked[j] = ranked[j - 1U]; --j;
        }
        ranked[j] = entry;
    }
    workspace->busy = true;
    status = umi_ai_reranker_apply(reranker != NULL ? &guarded : NULL, query, ranked, count);
    workspace->busy = false;
    if (status != UMI_STATUS_OK || guard.invalid) { free(ranked); return UMI_STATUS_INVALID_STATE; }
    for (size_t i = 0U; i < count && outputCount < capacity; ++i) {
        if (ranked[i].score <= 0.0) continue;
        results[outputCount].source = workspace->state->sources[ranked[i].chunk.index];
        results[outputCount++].score = ranked[i].score;
    }
    *outCount = outputCount; free(ranked); return UMI_STATUS_OK;
}
