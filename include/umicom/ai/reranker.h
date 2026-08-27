/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/reranker.h
 *
 * PURPOSE:
 *   Define a provider-neutral optional reranker callback over existing retrieval results.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_RERANKER_H
#define UMICOM_AI_RERANKER_H

#include "umicom/ai/retrieval.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef double (*UmiAiRerankScoreFn)(
    const char *query,
    const UmiAiChunk *chunk,
    double base_score,
    void *user_data);

typedef struct UmiAiReranker {
    UmiAiRerankScoreFn score;
    void *user_data;
} UmiAiReranker;

/* Apply an optional reranker and sort results in descending score order. */
UmiStatus umi_ai_reranker_apply(
    const UmiAiReranker *reranker,
    const char *query,
    UmiAiRetrievalResult *results,
    size_t result_count);

#ifdef __cplusplus
}
#endif
#endif
