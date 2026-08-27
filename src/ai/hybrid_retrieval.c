/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/hybrid_retrieval.c
 *
 * PURPOSE:
 *   Implement hybrid lexical/vector retrieval by composing existing retrieval and vector-store APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/hybrid_retrieval.h"

#include <string.h>

static size_t find_result(
    UmiAiRetrievalResult *results,
    size_t count,
    const char *chunk_id)
{
    size_t index;

    for (index = 0U; index < count; ++index) {
        if (strcmp(
                results[index].chunk.chunk_id,
                chunk_id) == 0) {
            return index;
        }
    }

    return SIZE_MAX;
}

static void insert_or_add(
    UmiAiRetrievalResult *results,
    size_t *count,
    size_t capacity,
    const UmiAiChunk *chunk,
    double score)
{
    size_t index = find_result(
        results,
        *count,
        chunk->chunk_id);

    if (index != SIZE_MAX) {
        results[index].score += score;
        return;
    }

    if (*count >= capacity) {
        return;
    }

    results[*count].chunk = *chunk;
    results[*count].score = score;
    *count += 1U;
}

UmiAiHybridRetrievalSettings
umi_ai_hybrid_retrieval_settings_default(void)
{
    UmiAiHybridRetrievalSettings settings;

    settings.lexical_weight = 0.5;
    settings.vector_weight = 0.5;
    settings.candidate_limit =
        UMI_AI_MAX_RETRIEVAL_RESULTS;

    return settings;
}

size_t umi_ai_hybrid_retrieval_rank(
    const char *query,
    const UmiAiChunk *chunks,
    size_t chunk_count,
    const UmiAiVectorStore *vector_store,
    const UmiAiEmbedding *query_embedding,
    const UmiAiHybridRetrievalSettings *settings,
    const UmiAiReranker *reranker,
    UmiAiRetrievalResult *results,
    size_t capacity)
{
    UmiAiHybridRetrievalSettings effective;
    UmiAiRetrievalResult
        lexical[UMI_AI_MAX_RETRIEVAL_RESULTS];
    UmiAiRetrievalResult
        candidates[UMI_AI_HYBRID_MAX_CANDIDATES];
    size_t vector_indices[UMI_AI_MAX_RETRIEVAL_RESULTS];
    double vector_scores[UMI_AI_MAX_RETRIEVAL_RESULTS];
    size_t lexical_count;
    size_t vector_count = 0U;
    size_t candidate_count = 0U;
    size_t index;
    size_t limit;
    size_t output_count;
    double lexical_maximum = 0.0;

    if (query == NULL ||
        chunks == NULL ||
        results == NULL ||
        capacity == 0U) {
        return 0U;
    }

    effective = settings != NULL
        ? *settings
        : umi_ai_hybrid_retrieval_settings_default();

    limit = effective.candidate_limit;
    if (limit == 0U ||
        limit > UMI_AI_MAX_RETRIEVAL_RESULTS) {
        limit = UMI_AI_MAX_RETRIEVAL_RESULTS;
    }

    lexical_count = umi_ai_retrieval_rank(
        query,
        chunks,
        chunk_count,
        lexical,
        limit);

    for (index = 0U; index < lexical_count; ++index) {
        if (lexical[index].score > lexical_maximum) {
            lexical_maximum = lexical[index].score;
        }
    }

    for (index = 0U; index < lexical_count; ++index) {
        double normalized = lexical_maximum > 0.0
            ? lexical[index].score / lexical_maximum
            : 0.0;

        insert_or_add(
            candidates,
            &candidate_count,
            UMI_AI_HYBRID_MAX_CANDIDATES,
            &lexical[index].chunk,
            normalized * effective.lexical_weight);
    }

    if (vector_store != NULL &&
        query_embedding != NULL) {
        vector_count = umi_ai_vector_store_search(
            vector_store,
            query_embedding,
            vector_indices,
            vector_scores,
            limit);

        for (index = 0U; index < vector_count; ++index) {
            size_t vector_index = vector_indices[index];
            double normalized;

            if (vector_index >= vector_store->count) {
                continue;
            }

            /*
             * Cosine similarity can be negative. Negative similarity does not
             * strengthen a hybrid candidate, so clamp it to the neutral floor.
             */
            normalized = vector_scores[index] > 0.0
                ? vector_scores[index]
                : 0.0;

            insert_or_add(
                candidates,
                &candidate_count,
                UMI_AI_HYBRID_MAX_CANDIDATES,
                &vector_store->entries[vector_index].chunk,
                normalized * effective.vector_weight);
        }
    }

    if (umi_ai_reranker_apply(
            reranker,
            query,
            candidates,
            candidate_count) != UMI_STATUS_OK) {
        return 0U;
    }

    output_count = candidate_count < capacity
        ? candidate_count
        : capacity;

    for (index = 0U; index < output_count; ++index) {
        results[index] = candidates[index];
    }

    return output_count;
}
