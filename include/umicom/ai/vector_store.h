/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/vector_store.h
 *
 * PURPOSE:
 *   Store a bounded set of chunk embeddings and perform top-match lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The bounded in-memory store is a deterministic reference adapter; future Chroma, SQLite or server stores can sit behind a provider interface.
 */

#ifndef INCLUDE_UMICOM_AI_VECTOR_STORE_H
#define INCLUDE_UMICOM_AI_VECTOR_STORE_H

#include <stddef.h>
#include "umicom/ai/embedding.h"
#include "umicom/ai/chunk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_VECTOR_STORE_CAPACITY 128U

/**
 * Represent the ai vector entry data shared with callers of this public contract.
 */
typedef struct UmiAiVectorEntry {
    UmiAiChunk chunk;
    UmiAiEmbedding embedding;
} UmiAiVectorEntry;

/**
 * Represent the ai vector store data shared with callers of this public contract.
 */
typedef struct UmiAiVectorStore {
    UmiAiVectorEntry entries[UMI_AI_VECTOR_STORE_CAPACITY];
    size_t count;
} UmiAiVectorStore;

/** Check count and every active chunk/embedding. Empty initialized stores are valid.
 * Unused entries are ignored. Mixed dimensions and duplicate chunks remain allowed. */
bool umi_ai_vector_store_valid(const UmiAiVectorStore *store);

/**
 * Initialise ai vector store from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_vector_store_init(UmiAiVectorStore *store);
/**
 * Add ai vector store only after its inputs and available capacity have been checked.
 * Invalid input returns INVALID_ARGUMENT; malformed stored state returns INVALID_STATE.
 * Refusal preserves the whole store. Input records may borrow from the store.
 */
UmiStatus umi_ai_vector_store_add(UmiAiVectorStore *store,
                                  const UmiAiChunk *chunk,
                                  const UmiAiEmbedding *embedding);
/**
 * Provide the ai vector store search operation used by this module and its client
 * applications. Returns zero without changing either output on invalid input or
 * stored state, or when capacity is zero. Outputs must be writable for capacity
 * entries and must not overlap each other, the store or query. Mutation during a
 * search requires external synchronization. A zero return is not an error code.
 * Results are descending, with earlier entries first on ties. Unequal dimensions
 * and zero-magnitude vectors retain a neutral cosine score rather than being skipped.
 */
size_t umi_ai_vector_store_search(const UmiAiVectorStore *store,
                                  const UmiAiEmbedding *query,
                                  size_t *indices,
                                  double *scores,
                                  size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
