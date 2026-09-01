/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/retrieval.h
 *
 * PURPOSE:
 *   Represent ranked retrieval results and basic lexical scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A simple lexical ranker gives RAG deterministic behaviour even when no embedding provider is configured.
 */

#ifndef INCLUDE_UMICOM_AI_RETRIEVAL_H
#define INCLUDE_UMICOM_AI_RETRIEVAL_H

#include <stddef.h>
#include "umicom/ai/chunk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiRetrievalResult {
    UmiAiChunk chunk;
    double score;
} UmiAiRetrievalResult;

double umi_ai_retrieval_lexical_score(const char *query, const char *text);
size_t umi_ai_retrieval_rank(const char *query,
                             const UmiAiChunk *chunks,
                             size_t chunk_count,
                             UmiAiRetrievalResult *results,
                             size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
