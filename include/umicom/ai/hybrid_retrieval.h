/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/hybrid_retrieval.h
 *
 * PURPOSE:
 *   Combine the existing lexical retriever and vector store into one optional hybrid retrieval path with reranking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_HYBRID_RETRIEVAL_H
#define UMICOM_AI_HYBRID_RETRIEVAL_H

#include "umicom/ai/reranker.h"
#include "umicom/ai/vector_store.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_HYBRID_MAX_CANDIDATES (UMI_AI_MAX_RETRIEVAL_RESULTS * 2U)

typedef struct UmiAiHybridRetrievalSettings {
    double lexical_weight;
    double vector_weight;
    size_t candidate_limit;
} UmiAiHybridRetrievalSettings;

UmiAiHybridRetrievalSettings
umi_ai_hybrid_retrieval_settings_default(void);

/*
 * Combine lexical scores over chunks with vector scores from the existing
 * vector store. query_embedding may be NULL to fall back to lexical-only
 * operation. No new index or source store is created.
 */
size_t umi_ai_hybrid_retrieval_rank(
    const char *query,
    const UmiAiChunk *chunks,
    size_t chunk_count,
    const UmiAiVectorStore *vector_store,
    const UmiAiEmbedding *query_embedding,
    const UmiAiHybridRetrievalSettings *settings,
    const UmiAiReranker *reranker,
    UmiAiRetrievalResult *results,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
