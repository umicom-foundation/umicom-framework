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

/* BEGINNER NOTE:
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

typedef struct UmiAiVectorEntry {
    UmiAiChunk chunk;
    UmiAiEmbedding embedding;
} UmiAiVectorEntry;

typedef struct UmiAiVectorStore {
    UmiAiVectorEntry entries[UMI_AI_VECTOR_STORE_CAPACITY];
    size_t count;
} UmiAiVectorStore;

void umi_ai_vector_store_init(UmiAiVectorStore *store);
UmiStatus umi_ai_vector_store_add(UmiAiVectorStore *store,
                                  const UmiAiChunk *chunk,
                                  const UmiAiEmbedding *embedding);
size_t umi_ai_vector_store_search(const UmiAiVectorStore *store,
                                  const UmiAiEmbedding *query,
                                  size_t *indices,
                                  double *scores,
                                  size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
