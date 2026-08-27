/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/reranker.c
 *
 * PURPOSE:
 *   Apply optional reranking to existing retrieval results while retaining deterministic ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/reranker.h"

static void sort_results(
    UmiAiRetrievalResult *results,
    size_t count)
{
    size_t index;

    for (index = 1U; index < count; ++index) {
        UmiAiRetrievalResult value = results[index];
        size_t position = index;

        while (position > 0U &&
               value.score > results[position - 1U].score) {
            results[position] = results[position - 1U];
            --position;
        }

        results[position] = value;
    }
}

UmiStatus umi_ai_reranker_apply(
    const UmiAiReranker *reranker,
    const char *query,
    UmiAiRetrievalResult *results,
    size_t result_count)
{
    size_t index;

    if (query == NULL ||
        (result_count > 0U && results == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (reranker != NULL && reranker->score != NULL) {
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
