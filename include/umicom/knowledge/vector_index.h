/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/vector_index.h
 * PURPOSE: Provide a bounded local vector index with metadata filtering.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: The index ranks similar vectors, then applies explicit metadata filters. */
#ifndef UMICOM_KNOWLEDGE_VECTOR_INDEX_H
#define UMICOM_KNOWLEDGE_VECTOR_INDEX_H

#include "umicom/knowledge/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiKnowledgeVectorEntry {
    UmiKnowledgeChunk chunk;
    UmiKnowledgeEmbedding embedding;
} UmiKnowledgeVectorEntry;

typedef struct UmiKnowledgeVectorIndex UmiKnowledgeVectorIndex;

UmiStatus umi_knowledge_vector_index_create(
    size_t capacity,
    size_t dimension,
    UmiKnowledgeDistanceMetric metric,
    UmiKnowledgeVectorIndex **out_index);
void umi_knowledge_vector_index_destroy(UmiKnowledgeVectorIndex *index);
UmiStatus umi_knowledge_vector_index_upsert(
    UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeChunk *chunk,
    const UmiKnowledgeEmbedding *embedding);
UmiStatus umi_knowledge_vector_index_remove_source(
    UmiKnowledgeVectorIndex *index,
    const char *source_id,
    size_t *out_removed);
UmiStatus umi_knowledge_vector_index_search(
    const UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbedding *query,
    const UmiKnowledgeFilter *filter,
    size_t limit,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count);
size_t umi_knowledge_vector_index_count(
    const UmiKnowledgeVectorIndex *index);
size_t umi_knowledge_vector_index_dimension(
    const UmiKnowledgeVectorIndex *index);
UmiStatus umi_knowledge_vector_index_entry_at(
    const UmiKnowledgeVectorIndex *index,
    size_t position,
    UmiKnowledgeVectorEntry *out_entry);

#ifdef __cplusplus
}
#endif
#endif
