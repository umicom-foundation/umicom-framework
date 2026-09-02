/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/reranker.c
 *
 * PURPOSE:
 *   Apply optional reranking to existing retrieval results while retaining deterministic ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/reranker.h"

/* Provide the sort results operation used by this module and its client applications. */
static void sort_results(
    UmiAiRetrievalResult *results,
    size_t count)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < count; ++index) {
        UmiAiRetrievalResult value = results[index];
        size_t position = index;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position > 0U &&
               value.score > results[position - 1U].score) {
            results[position] = results[position - 1U];
            --position;
        }

        results[position] = value;
    }
}

/*
 * Perform ai reranker through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_ai_reranker_apply(
    const UmiAiReranker *reranker,
    const char *query,
    UmiAiRetrievalResult *results,
    size_t result_count)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL ||
        (result_count > 0U && results == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (reranker != NULL && reranker->score != NULL) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < result_count; ++index) {
            results[index].score = reranker->score(
                query,
                &results[index].chunk,
                results[index].score,
                reranker->user_data);
        }
    }

    sort_results(results, result_count);
    return UMI_STATUS_OK;
}
