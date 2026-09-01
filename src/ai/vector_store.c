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

void umi_ai_vector_store_init(UmiAiVectorStore *store)
{
    if (store != NULL) {
        (void)memset(store, 0, sizeof(*store));
    }
}

UmiStatus umi_ai_vector_store_add(UmiAiVectorStore *store,
                                  const UmiAiChunk *chunk,
                                  const UmiAiEmbedding *embedding)
{
    if (store == NULL || chunk == NULL || embedding == NULL ||
        embedding->dimension == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (store->count >= UMI_AI_VECTOR_STORE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->entries[store->count].chunk = *chunk;
    store->entries[store->count].embedding = *embedding;
    ++store->count;
    return UMI_STATUS_OK;
}

size_t umi_ai_vector_store_search(const UmiAiVectorStore *store,
                                  const UmiAiEmbedding *query,
                                  size_t *indices,
                                  double *scores,
                                  size_t capacity)
{
    size_t i;
    size_t count = 0U;
    if (store == NULL || query == NULL || indices == NULL || scores == NULL) {
        return 0U;
    }
    for (i = 0U; i < store->count; ++i) {
        double score = umi_ai_embedding_cosine(query, &store->entries[i].embedding);
        size_t pos = count < capacity ? count : capacity;
        if (capacity == 0U) break;
        if (count < capacity) ++count;
        else if (score <= scores[capacity - 1U]) continue;
        if (pos >= capacity) pos = capacity - 1U;
        while (pos > 0U && score > scores[pos - 1U]) {
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
