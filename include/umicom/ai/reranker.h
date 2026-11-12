/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/reranker.h
 *
 * PURPOSE:
 *   Define a provider-neutral optional reranker callback over existing retrieval results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_RERANKER_H
#define UMICOM_AI_RERANKER_H

#include "umicom/ai/retrieval.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return one finite score. Query, chunk and user data are borrowed for the call.
 * Do not retain the chunk pointer, cast away const, mutate the result owner or
 * reenter a mutation of that owner. External callback side effects cannot roll back. */
typedef double (*UmiAiRerankScoreFn)(
    const char *query,
    const UmiAiChunk *chunk,
    double base_score,
    void *user_data);

/**
 * Represent the ai reranker data shared with callers of this public contract.
 */
typedef struct UmiAiReranker {
    UmiAiRerankScoreFn score;
    void *user_data;
} UmiAiReranker;

/* Apply an optional reranker and sort results in descending score order.
 * Invalid input records return INVALID_ARGUMENT before any callback or mutation.
 * A non-finite callback score returns INVALID_STATE; allocation failure returns
 * OUT_OF_MEMORY. These refusals preserve all result bytes, provided the callback obeys
 * its ownership contract. Null callback performs a stable sort without allocation.
 * Count zero accepts null results. Query must be a terminated C string; callers
 * provide the complete advertised array extent and exclude concurrent mutation. */
UmiStatus umi_ai_reranker_apply(
    const UmiAiReranker *reranker,
    const char *query,
    UmiAiRetrievalResult *results,
    size_t result_count);

#ifdef __cplusplus
}
#endif
#endif
