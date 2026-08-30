/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/vector_index.c
 * PURPOSE: Implement a deterministic local vector index and ranked filtering.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/vector_index.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

struct UmiKnowledgeVectorIndex {
    UmiKnowledgeVectorEntry *entries;
    size_t count;
    size_t capacity;
    size_t dimension;
    UmiKnowledgeDistanceMetric metric;
};

UmiStatus umi_knowledge_vector_index_create(
    size_t capacity,
    size_t dimension,
    UmiKnowledgeDistanceMetric metric,
    UmiKnowledgeVectorIndex **out_index)
{
    UmiKnowledgeVectorIndex *index;
    if (capacity == 0U || dimension == 0U ||
        dimension > UMI_KNOWLEDGE_EMBEDDING_MAX ||
        (metric != UMI_KNOWLEDGE_DISTANCE_COSINE &&
         metric != UMI_KNOWLEDGE_DISTANCE_DOT) || out_index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_index = NULL;
    index = (UmiKnowledgeVectorIndex *)calloc(1U, sizeof(*index));
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->entries = (UmiKnowledgeVectorEntry *)calloc(
        capacity, sizeof(*index->entries));
    if (index->entries == NULL) {
        free(index);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    index->capacity = capacity;
    index->dimension = dimension;
    index->metric = metric;
    *out_index = index;
    return UMI_STATUS_OK;
}

void umi_knowledge_vector_index_destroy(UmiKnowledgeVectorIndex *index)
{
    if (index == NULL) return;
    free(index->entries);
    free(index);
}

UmiStatus umi_knowledge_vector_index_upsert(
    UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeChunk *chunk,
    const UmiKnowledgeEmbedding *embedding)
{
    size_t position;
    if (index == NULL || chunk == NULL || embedding == NULL ||
        chunk->chunk_id[0] == '\0' ||
        embedding->dimension != index->dimension) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (position = 0U; position < index->count; ++position) {
        if (strcmp(index->entries[position].chunk.chunk_id,
                   chunk->chunk_id) == 0) {
            index->entries[position].chunk = *chunk;
            index->entries[position].embedding = *embedding;
            return UMI_STATUS_OK;
        }
    }
    if (index->count >= index->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    index->entries[index->count].chunk = *chunk;
    index->entries[index->count].embedding = *embedding;
    ++index->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_knowledge_vector_index_remove_source(
    UmiKnowledgeVectorIndex *index,
    const char *source_id,
    size_t *out_removed)
{
    size_t read_position;
    size_t write_position = 0U;
    size_t removed = 0U;
    if (index == NULL || source_id == NULL || source_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_position = 0U; read_position < index->count; ++read_position) {
        if (strcmp(index->entries[read_position].chunk.source_id,
                   source_id) == 0) {
            ++removed;
            continue;
        }
        if (write_position != read_position) {
            index->entries[write_position] = index->entries[read_position];
        }
        ++write_position;
    }
    index->count = write_position;
    if (out_removed != NULL) *out_removed = removed;
    return UMI_STATUS_OK;
}

static double similarity(const UmiKnowledgeEmbedding *left,
                         const UmiKnowledgeEmbedding *right,
                         UmiKnowledgeDistanceMetric metric)
{
    double dot = 0.0;
    double left_norm = 0.0;
    double right_norm = 0.0;
    size_t index;
    for (index = 0U; index < left->dimension; ++index) {
        double a = (double)left->values[index];
        double b = (double)right->values[index];
        dot += a * b;
        left_norm += a * a;
        right_norm += b * b;
    }
    if (metric == UMI_KNOWLEDGE_DISTANCE_DOT) return dot;
    if (left_norm == 0.0 || right_norm == 0.0) return 0.0;
    return dot / (sqrt(left_norm) * sqrt(right_norm));
}

static void insert_match(UmiKnowledgeMatch *matches, size_t *count,
                         size_t capacity, const UmiKnowledgeChunk *chunk,
                         double score)
{
    size_t position = *count < capacity ? *count : capacity - 1U;
    if (*count < capacity) ++*count;
    while (position > 0U && matches[position - 1U].score < score) {
        if (position < capacity) matches[position] = matches[position - 1U];
        --position;
    }
    if (position < capacity) {
        (void)memset(&matches[position], 0, sizeof(matches[position]));
        matches[position].chunk = *chunk;
        matches[position].score = score;
    }
}

UmiStatus umi_knowledge_vector_index_search(
    const UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbedding *query,
    const UmiKnowledgeFilter *filter,
    size_t limit,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count)
{
    size_t position;
    size_t count = 0U;
    size_t result_capacity;
    if (index == NULL || query == NULL || matches == NULL ||
        capacity == 0U || out_count == NULL || limit == 0U ||
        query->dimension != index->dimension) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    result_capacity = limit < capacity ? limit : capacity;
    (void)memset(matches, 0, capacity * sizeof(*matches));
    for (position = 0U; position < index->count; ++position) {
        double score;
        if (!umi_knowledge_filter_matches(
                filter, &index->entries[position].chunk)) continue;
        score = similarity(query, &index->entries[position].embedding,
                           index->metric);
        if (count < result_capacity ||
            score > matches[result_capacity - 1U].score) {
            insert_match(matches, &count, result_capacity,
                         &index->entries[position].chunk, score);
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}

size_t umi_knowledge_vector_index_count(const UmiKnowledgeVectorIndex *index)
{
    return index != NULL ? index->count : 0U;
}

size_t umi_knowledge_vector_index_dimension(
    const UmiKnowledgeVectorIndex *index)
{
    return index != NULL ? index->dimension : 0U;
}

UmiStatus umi_knowledge_vector_index_entry_at(
    const UmiKnowledgeVectorIndex *index,
    size_t position,
    UmiKnowledgeVectorEntry *out_entry)
{
    if (index == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = index->entries[position];
    return UMI_STATUS_OK;
}
