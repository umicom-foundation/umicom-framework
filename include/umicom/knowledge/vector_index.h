/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/vector_index.h
 * PURPOSE: Provide a bounded local vector index with metadata filtering.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The index ranks similar vectors, then applies explicit metadata filters. */
#ifndef UMICOM_KNOWLEDGE_VECTOR_INDEX_H
#define UMICOM_KNOWLEDGE_VECTOR_INDEX_H

#include "umicom/knowledge/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the knowledge vector entry data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeVectorEntry {
    UmiKnowledgeChunk chunk;
    UmiKnowledgeEmbedding embedding;
} UmiKnowledgeVectorEntry;

/**
 * Represent the knowledge vector index data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeVectorIndex UmiKnowledgeVectorIndex;

/**
 * Initialise knowledge vector index from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_knowledge_vector_index_create(
    size_t capacity,
    size_t dimension,
    UmiKnowledgeDistanceMetric metric,
    UmiKnowledgeVectorIndex **out_index);
/**
 * Release or reset state held by knowledge vector index so the same storage can be reused
 * safely.
 */
void umi_knowledge_vector_index_destroy(UmiKnowledgeVectorIndex *index);
/**
 * Provide the knowledge vector index upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_vector_index_upsert(
    UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeChunk *chunk,
    const UmiKnowledgeEmbedding *embedding);
/**
 * Provide the knowledge vector index remove source operation used by this module and its
 * client applications.
 */
UmiStatus umi_knowledge_vector_index_remove_source(
    UmiKnowledgeVectorIndex *index,
    const char *source_id,
    size_t *out_removed);
/**
 * Provide the knowledge vector index search operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_vector_index_search(
    const UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbedding *query,
    const UmiKnowledgeFilter *filter,
    size_t limit,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count);
/**
 * Return the number of records represented by knowledge vector index without changing
 * their state.
 */
size_t umi_knowledge_vector_index_count(
    const UmiKnowledgeVectorIndex *index);
/**
 * Provide the knowledge vector index dimension operation used by this module and its
 * client applications.
 */
size_t umi_knowledge_vector_index_dimension(
    const UmiKnowledgeVectorIndex *index);
/**
 * Find knowledge vector index entry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_knowledge_vector_index_entry_at(
    const UmiKnowledgeVectorIndex *index,
    size_t position,
    UmiKnowledgeVectorEntry *out_entry);

#ifdef __cplusplus
}
#endif
#endif
