/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/vector_store.c
 *
 * PURPOSE:
 *   Maintain and search a bounded in-memory embedding index.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Search keeps the best scores in descending order so callers can request a small top-k result without sorting the entire store.
 */

#include "umicom/ai/vector_store.h"
#include <stddef.h>

#include <string.h>

/* Validate every active row before any search output or insertion is published.
 * A malformed later row must not be hidden by an earlier high-scoring result. */
bool umi_ai_vector_store_valid(const UmiAiVectorStore *store)
{
    if (store == NULL || store->count > UMI_AI_VECTOR_STORE_CAPACITY) return false;
    for (size_t index = 0U; index < store->count; ++index)
        if (!umi_ai_chunk_storage_valid(&store->entries[index].chunk) ||
            !umi_ai_embedding_valid(&store->entries[index].embedding)) return false;
    return true;
}


/*
 * Initialise ai vector store from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_vector_store_init(UmiAiVectorStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) {
        (void)memset(store, 0, sizeof(*store));
    }
}

/* Add ai vector store only after its inputs and available capacity have been checked. */
/* Whole-store admission and a staged entry preserve alias ownership and refuse malformed records before publication; the previous insertion is The previous implementation is retained for review. */
#if 0
UmiStatus umi_ai_vector_store_add(UmiAiVectorStore *store,
                                  const UmiAiChunk *chunk,
                                  const UmiAiEmbedding *embedding)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || chunk == NULL || embedding == NULL ||
        embedding->dimension == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_AI_VECTOR_STORE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->entries[store->count].chunk = *chunk;
    store->entries[store->count].embedding = *embedding;
    ++store->count;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ai_vector_store_add(UmiAiVectorStore *store,
                                  const UmiAiChunk *chunk,
                                  const UmiAiEmbedding *embedding)
{
    UmiAiVectorEntry candidate = {0};
    if (store == NULL || !umi_ai_chunk_storage_valid(chunk) ||
        !umi_ai_embedding_valid(embedding)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_ai_vector_store_valid(store)) return UMI_STATUS_INVALID_STATE;
    if (store->count == UMI_AI_VECTOR_STORE_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Capture both fields first: callers can borrow an existing or unused entry.
     * Only the new slot and count change after all admission checks succeed. */
    candidate.chunk = *chunk;
    candidate.embedding = *embedding;
    (void)memcpy(&store->entries[store->count], &candidate, sizeof(candidate));
    ++store->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai vector store search operation used by this module and its client
 * applications.
 */
/* Complete bounded admission now precedes output publication and zero-capacity work; descending scores and stable ties preserve the previous ranking policy, whose implementation is The previous implementation is retained for review. */
#if 0
size_t umi_ai_vector_store_search(const UmiAiVectorStore *store,
                                  const UmiAiEmbedding *query,
                                  size_t *indices,
                                  double *scores,
                                  size_t capacity)
{
    size_t i;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || query == NULL || indices == NULL || scores == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < store->count; ++i) {
        double score = umi_ai_embedding_cosine(query, &store->entries[i].embedding);
        size_t pos = count < capacity ? count : capacity;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity == 0U) break;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count < capacity) ++count;
        else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (score <= scores[capacity - 1U]) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (pos >= capacity) pos = capacity - 1U;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (pos > 0U && score > scores[pos - 1U]) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (pos < capacity) {
                scores[pos] = scores[pos - 1U];
                indices[pos] = indices[pos - 1U];
            }
            --pos;
        }
        scores[pos] = score;
        indices[pos] = i;
    }
    return count;
}
#endif
size_t umi_ai_vector_store_search(const UmiAiVectorStore *store,
                                  const UmiAiEmbedding *query,
                                  size_t *indices,
                                  double *scores,
                                  size_t capacity)
{
    size_t count = 0U;
    /* Zero requested output is a no-op, even when no other argument is present.
     * Whole-record admission precedes writes, including validation of late rows. */
    if (capacity == 0U || indices == NULL || scores == NULL ||
        !umi_ai_embedding_valid(query) || !umi_ai_vector_store_valid(store))
        return 0U;
    for (size_t index = 0U; index < store->count; ++index) {
        double score = umi_ai_embedding_cosine(query, &store->entries[index].embedding);
        size_t position = count;
        if (count < capacity) ++count;
        else {
            if (score <= scores[capacity - 1U]) continue;
            position = capacity - 1U;
        }
        /* Strict comparison retains input order for tied scores. Different
         * dimensions and zero vectors keep the existing neutral-score policy. */
        while (position > 0U && score > scores[position - 1U]) {
            scores[position] = scores[position - 1U];
            indices[position] = indices[position - 1U];
            --position;
        }
        scores[position] = score;
        indices[position] = index;
    }
    return count;
}
