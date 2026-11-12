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

/**
 * Represent the ai retrieval result data shared with callers of this public contract.
 */
typedef struct UmiAiRetrievalResult {
    UmiAiChunk chunk;
    double score;
} UmiAiRetrievalResult;

/** Admit complete counted arrays before scoring or publication. Count zero
 * permits a null array. Callers still own allocation extent and synchronization. */
bool umi_ai_retrieval_chunks_valid(const UmiAiChunk *chunks, size_t count);
bool umi_ai_retrieval_results_valid(const UmiAiRetrievalResult *results, size_t count);

/**
 * Provide the ai retrieval lexical score operation used by this module and its client
 * applications. Both inputs must be terminated C strings. Scoring uses the
 * current C locale for case folding; it is not Unicode linguistic matching.
 */
double umi_ai_retrieval_lexical_score(const char *query, const char *text);
/**
 * Provide the ai retrieval rank operation used by this module and its client applications.
 * All chunk strings are admitted before any output; malformed input returns zero
 * with results unchanged. Query must be a terminated C string. Caller arrays must
 * have the advertised extents and results must not overlap chunks or query.
 * Equal scores retain input order; zero count also represents no matching chunks.
 */
size_t umi_ai_retrieval_rank(const char *query,
                             const UmiAiChunk *chunks,
                             size_t chunk_count,
                             UmiAiRetrievalResult *results,
                             size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
