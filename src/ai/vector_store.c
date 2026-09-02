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

/*
 * Provide the ai vector store search operation used by this module and its client
 * applications.
 */
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
